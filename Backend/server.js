const http = require('http');
const express = require('express')
const cors = require('cors')
const path = require('path')
const { v4: uuidv4 } = require('uuid')
const { Bot } = require('./build/Release/greet.node')

const app = express()
app.use(cors())
app.use(express.json({ limit: '50mb' }))

const hostname = '0.0.0.0'
const port = 3000

const bot = new Bot()
// console.log(bot.playTurn("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"))

async function playTurn(fen, difficulty) {
	return await Promise.resolve(bot.playTurn(fen, difficulty));
}

async function getLegalMoves(fen) {
	return await Promise.resolve(bot.getLegalMoves(fen));
}

async function getBestMove(fen) {
	return await Promise.resolve(bot.getBestMove(fen));
}

app.post('/playTurn', async (request, response, next) => {
	const { body } = request
	console.log('body: ', body)
	playTurn(body.fen, body.difficulty)
		.then((result) => {
			console.log('result: ', result)
			response.json(result)
		})
		.catch((error) => next(error))
})

app.post('/getLegalMoves', async (request, response, next) => {
	const { body } = request
	console.log('ask moves body: ', body)
	getLegalMoves(body.fen)
		.then((result) => {
			console.log('result: ', result)
			response.json(result)
		})
		.catch((error) => next(error))
})

app.post('/getBestMove', async (request, response, next) => {
	const { body } = request
	console.log('hint body: ', body)
	getBestMove(body.fen)
		.then((result) => {
			console.log('result: ', result)
			response.json(result)
		})
		.catch((error) => next(error))
})

const pvpgames = {}

app.post('/startPVP', async (request, response, next) => {
	const { body } = request

	console.log('start pvp body: ', body)

	const playerId = body.playerId
	const gameId = uuidv4()
	const game = pvpgames[gameId]

	if (gameId in pvpgames) {
		console.log('game found', gameId)
		if (!pvpgames[gameId].p1 === playerId && body.color === 'white') {
			//join game as white
			pvpgames[gameId].p1 = playerId
		} else if (!pvpgames[gameId].p2 === playerId && body.color === 'black') {
			//join game as black
			pvpgames[gameId].p2 = playerId
		}
	} else {
		console.log('game created', gameId)
		//create game

		pvpgames[gameId] = {
			turn: 0,
			p1: body.color === "white" ? playerId : null,
			p2: body.color === "black" ? playerId : null,
			fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
			lastMove: ""
		}
		console.log('game data', pvpgames[gameId])
	}
	response.json({ gameId })
})
app.post('/joinRandomPVP', async (request, response, next) => {
	const { body } = request

	console.log('join random pvp body: ', body)

	const playerId = body.playerId

	const games = Object.keys(pvpgames).filter((gameId) => (pvpgames[gameId].p1 !== null && pvpgames[gameId].p2 === null))
	if(games.length === 0){
		response.status(404).json({ error: 'Couldn\'t find a game'})
	}
	response.json({ gameId: games[0] })
})

app.post('/pvp/move/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	const fen = body.fen
	const move = body.move
	console.log('move body', body)

	const game = pvpgames[gameId]

	if (game === undefined) {
		response.status(404).json({ error: 'Couldn\'t find game with id ' + gameID })
		return
	}
	if (game.p1 === playerId) {
		if (game.turn % 2 === 0) {
			console.log('p1 move')
			game.fen = fen
			game.lastMove = move
			response.status(200).json({ data: 'Moved succesfully' })
		} else {
			response.status(403).json({ error: 'Wait for your turn, dummy' })
			return
		}
	} else if (game.p2 === playerId) {
		if (game.turn % 2 === 1) {
			console.log('p2 move')
			game.fen = fen
			game.lastMove = move
			response.status(200).json({ data: 'Moved succesfully' })
		} else {
			response.status(403).json({ error: 'Wait for your turn, dummy' })
			return
		}
	} else {
		response.status(401).json({ error: 'You are not a part of this game' })
		return
	}

	game.turn++
})

app.get('/pvp/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	console.log('stream body', body)
	console.log('gameId', request.params.gameId)
	console.log('playerId', request.params.playerId)

	response.set({
		// needed for SSE!
		'Content-Type': 'text/event-stream',
		'Cache-Control': 'no-cache',
		Connection: 'keep-alive',

		// enabling CORS
		'Access-Control-Allow-Origin': '*',
		'Access-Control-Allow-Headers':
			'Origin, X-Requested-With, Content-Type, Accept',
	})
	response.flush()

	if (gameId in pvpgames) {
		console.log('game found', gameId)
		if (!(pvpgames[gameId].p1 === playerId || pvpgames[gameId].p2 === playerId)) {
			//join game
			pvpgames[gameId].p2 = playerId
			pvpgames[gameId].p2connected = true
		}
	} else {
		console.log('game created', gameId)
		//create game
		pvpgames[gameId] = {
			turn: 0,
			p1: playerId,
			p1connected: true,
			p2: null,
			p2connected: false,
			fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
			lastMove: ""
		}
		console.log('game data', pvpgames[gameId])
	}

	let lastTurn = -1
	const interValID = setInterval(() => {
		const game = pvpgames[gameId]
		if (game.p1 !== null && game.p2 !== null) {
			if (game.turn !== lastTurn) {
				console.log('updating')
				console.log('game data', game)
				console.log('lastTurn', lastTurn)
				getLegalMoves(game.fen)
					.then((moves) => {
						response.write(`data: ${JSON.stringify({ fen: game.fen, lastMove: game.lastMove, legalMoves: moves })}`)
						response.write('\n\n')
						response.flush()
					})
					.catch((error) => next(error))
				lastTurn = game.turn
			}
		} else {
			response.write(`data: "Waiting for player 2" \n\n`)
			response.flush()
		}
	}, 1000)

	// If client closes connection, stop sending events
	response.on('close', () => {
		console.log('client dropped me')
		clearInterval(interValID)
		response.end()
		const game = pvpgames[gameId]
		if(game.p1 === playerId){
			game.p1connected = false
		}else if(game.p2 === playerId){
			game.p2connected = false
		}
		if(!game.p1connected && !game.p2connected){
			delete pvpgames[gameId]
		}
	})
})

app.get('/matches', async (request, response, next) => {
	console.log('pvpgames', Object.values(pvpgames))
	const games = Object.values(pvpgames).filter((game) => game.p1 && game.p2)
	console.log('matches', games.length)
	response.status(200).json(`${games.length}`)
})

app.get('/matchmaking', async (request, response, next) => {
	console.log('pvpgames', Object.values(pvpgames))
	const games = Object.values(pvpgames).filter((game) => (game.p1 === null && game.p2 !== null) || (game.p1 !== null && game.p2 === null))
	console.log('matchmaking', games)
	response.status(200).json(`${games.length}`)
})

app.use(express.static(path.join(__dirname, '../Frontend/react-chess/dist')))

app.listen(port, hostname, () => {
	console.log(`Server running at http://${hostname}:${port}/`);
});