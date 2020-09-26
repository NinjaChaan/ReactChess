const http = require('http');
const express = require('express')
const cors = require('cors')
const path = require('path')
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
	console.log('body: ', body)
	getLegalMoves(body.fen)
		.then((result) => {
			console.log('result: ', result)
			response.json(result)
		})
		.catch((error) => next(error))
})
app.post('/getBestMove', async (request, response, next) => {
	const { body } = request
	console.log('body: ', body)
	getBestMove(body.fen)
		.then((result) => {
			console.log('result: ', result)
			response.json(result)
		})
		.catch((error) => next(error))
})


app.use(express.static(path.join(__dirname, '../Frontend/react-chess/dist')))

app.listen(port, hostname, () => {
	console.log(`Server running at http://${hostname}:${port}/`);
});