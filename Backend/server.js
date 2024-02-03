const http = require('http');
const express = require('express')
const cors = require('cors')
const path = require('path')
const { v4: uuidv4 } = require('uuid')
const { Bot } = require('./build/Release/shakki.node')

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

async function isGameOver(fen) {
	return await Promise.resolve(bot.isGameOver(fen));
}

app.post('/playTurn', async (request, response, next) => {
	const { body } = request
	playTurn(body.fen, body.difficulty)
		.then((result) => {
			response.json(result)
		})
		.catch((error) => next(error))
})


app.post('/getLegalMoves', async (request, response, next) => {
	const { body } = request
	getLegalMoves(body.fen)
		.then((result) => {
			response.json(result)
		})
		.catch((error) => next(error))
})

app.post('/getBestMove', async (request, response, next) => {
	const { body } = request
	getBestMove(body.fen)
		.then((result) => {
			response.json(result)
		})
		.catch((error) => next(error))
})

const pvpgames = {}

app.post('/startPVP', async (request, response, next) => {
	const { body } = request
	const playerId = body.playerId
	const gameId = uuidv4()

	if (gameId in pvpgames) {
		if (!pvpgames[gameId].p1 === playerId && body.color === 'white') {
			//join game as white
			pvpgames[gameId].p1 = playerId
		} else if (!pvpgames[gameId].p2 === playerId && body.color === 'black') {
			//join game as black
			pvpgames[gameId].p2 = playerId
		}
	} else {
		//create game
		pvpgames[gameId] = {
			turn: 0,
			p1: body.color === "white" ? playerId : null,
			p2: body.color === "black" ? playerId : null,
			fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
			status: "created",
			lastMove: ""
		}
	}
	response.json({ gameId })
})
app.post('/joinRandomPVP', async (request, response, next) => {
	const { body } = request

	const playerId = body.playerId

	const games = Object.keys(pvpgames).filter((gameId) => (pvpgames[gameId].p1 !== null && pvpgames[gameId].p2 === null))
	if (games.length === 0) {
		response.status(404).json({ error: 'Couldn\'t find a game' })
		return
	}
	response.json({ gameId: games[0] })
})



app.post('/pvp/move/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	const fen = body.fen
	const move = body.move

	const game = pvpgames[gameId]

	if (game === undefined) {
		response.status(404).json({ error: 'Couldn\'t find game with id ' + gameID })
		return
	}
	if (game.status.includes('resigned')) {
		response.status(403).json('Game is already over')
		return
	}
	if (game.p1 === playerId) {
		if (game.turn % 2 === 0) {
			console.log('p1 move')
			game.fen = fen
			game.lastMove = move
			game.status = 'ongoing'
			isGameOver(game.fen)
				.then((result) => {
					if (result.includes("wins") || result.includes("Draw")) {
						game.status = result
					}
				})
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
			game.status = 'ongoing'
			isGameOver(game.fen)
				.then((result) => {
					if (result.includes("wins") || result.includes("Draw")) {
						game.status = result
					}
				})
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

app.post('/pvp/resign/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	console.log('resign body', body)

	const game = pvpgames[gameId]

	if (game === undefined) {
		response.status(404).json({ error: 'Couldn\'t find game with id ' + gameID })
		return
	}
	if (game.p1 === playerId) {
		console.log('p1 resign')
		game.status = 'White resigned'
		response.status(200).json({ data: 'Resigned succesfully' })
	} else if (game.p2 === playerId) {
		console.log('p2 resign')
		game.status = 'Black resigned'
		response.status(200).json({ data: 'Resigned succesfully' })
	} else {
		response.status(401).json({ error: 'You are not a part of this game' })
		return
	}
})

app.post('/pvp/offerdraw/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	console.log('offer draw body', body)

	const game = pvpgames[gameId]

	if (game === undefined) {
		response.status(404).json({ error: 'Couldn\'t find game with id ' + gameID })
		return
	}
	if(game.status == 'reject'){
		response.status(402).json({ data: 'You cannot ask for a draw again.' })
		return
	}
	if (game.p1 === playerId) {
		console.log('p1 ask for draw')
		if (game.status == 'black offer') {
			game.status = 'Draw'
		}else{
			game.status = 'white offer'
		}
		response.status(200).json({ data: 'Asked draw succesfully' })
	} else if (game.p2 === playerId) {
		console.log('p2 ask for draw')
		if (game.status == 'white offer') {
			game.status = 'Draw'
		}else{
			game.status = 'black offer'
		}
		response.status(200).json({ data: 'Asked draw succesfully' })
	} else {
		response.status(401).json({ error: 'You are not a part of this game' })
		return
	}
})

app.post('/pvp/rejectdraw/:gameId/:playerId', async (request, response, next) => {
	const { body } = request
	const gameId = request.params.gameId
	const playerId = request.params.playerId
	console.log('reject draw body', body)

	const game = pvpgames[gameId]

	if (game === undefined) {
		response.status(404).json({ error: 'Couldn\'t find game with id ' + gameID })
		return
	}
	if (game.p1 === playerId) {
		console.log('p1 reject draw')
		game.status = 'reject'
		response.status(200).json({ data: 'Rejected draw succesfully' })
	} else if (game.p2 === playerId) {
		console.log('p2 reject draw')
		game.status = 'reject'
		response.status(200).json({ data: 'Rejected draw succesfully' })
	} else {
		response.status(401).json({ error: 'You are not a part of this game' })
		return
	}
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
		if (pvpgames[gameId].p1 === playerId) {
			pvpgames[gameId].p1connected = true
		} else if (pvpgames[gameId].p2 === playerId){
			pvpgames[gameId].p2connected = true
		}

		if (!(pvpgames[gameId].p1 === playerId || pvpgames[gameId].p2 === playerId)) {
			//join game
			pvpgames[gameId].p2 = playerId
			pvpgames[gameId].p2connected = true
			pvpgames[gameId].status = "ongoing"
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
			status: 'created',
			fen: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
			lastMove: ""
		}
		console.log('game data', pvpgames[gameId])
	}

	let lastTurn = -1
	let lastStatus = ""
	const interValID = setInterval(() => {
		const game = pvpgames[gameId]
		if (game) {
			if (game.p1 !== null && game.p2 !== null) {
				console.log('game fen', game.fen)
				if (game.turn !== lastTurn || game.status != lastStatus) {
					console.log('updating')
					console.log('game data', game)
					console.log('lastTurn', lastTurn)
					if (game.status.includes("wins") || game.status.includes("resigned") || game.status.includes("Draw")) {
						response.write(`data: ${JSON.stringify({ fen: game.fen, status: game.status, lastMove: game.lastMove })}`)
						response.write('\n\n')
						response.flush()
					} else {
						getLegalMoves(game.fen)
							.then((moves) => {
								response.write(`data: ${JSON.stringify({ fen: game.fen, status: game.status, lastMove: game.lastMove, legalMoves: moves })}`)
								response.write('\n\n')
								response.flush()
							})
							.catch((error) => next(error))
					}
					lastTurn = game.turn
					lastStatus = game.status
				}
			} else {
				response.write(`data: "Waiting for player 2" \n\n`)
				response.flush()
			}
		}
	}, 1000)

	// If client closes connection, stop sending events
	response.on('close', () => {
		console.log('client dropped me')
		clearInterval(interValID)
		response.end()
		const game = pvpgames[gameId]
		if (game) {
			if (game.p1 === playerId) {
				game.p1connected = false
			} else if (game.p2 === playerId) {
				game.p2connected = false
			}
			if (!game.p1connected && !game.p2connected) {
				delete pvpgames[gameId]
			}
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