// Runs the wasm engine in a Web Worker.

const engineBase = () => new URL('engine/', document.baseURI).href

// The worker runs from a blob: URL, so self.location can't resolve
// shakki.wasm / shakki.data. locateFile pins them to the engine directory.
const workerSource = () => `
const BASE = ${JSON.stringify(engineBase())};
importScripts(BASE + 'shakki.js');
let mod = null;
const ready = createChessEngine({ locateFile: (p) => BASE + p }).then((m) => { mod = m; });
self.onmessage = async (e) => {
	const { id, op, args } = e.data;
	try {
		await ready;
		self.postMessage({ id, result: mod[op].apply(mod, args) });
	} catch (err) {
		self.postMessage({ id, error: String((err && err.message) || err) });
	}
};
`

let worker = null
let seq = 0
const pending = new Map()

const getWorker = () => {
	if (worker) return worker

	const blob = new Blob([workerSource()], { type: 'text/javascript' })
	worker = new Worker(URL.createObjectURL(blob))
	worker.onmessage = (e) => {
		const { id, result, error } = e.data
		const entry = pending.get(id)
		if (!entry) return
		pending.delete(id)
		if (error) entry.reject(new Error(error))
		else entry.resolve(result)
	}
	return worker
}

const call = (op, args) => new Promise((resolve, reject) => {
	const id = ++seq
	pending.set(id, { resolve, reject })
	getWorker().postMessage({ id, op, args })
})

const withNumericKeys = (target, moves) => {
	moves.forEach((move, i) => { target[i] = move })
	return target
}

export const playTurn = ({ fen, difficulty }) =>
	call('playTurn', [fen, difficulty]).then((json) => {
		const r = JSON.parse(json)
		return {
			data: withNumericKeys(
				{ fen: r.fen, lastMove: r.lastMove, enPassant: r.enPassant },
				r.moves
			)
		}
	})

export const getLegalMoves = ({ fen }) =>
	call('getLegalMoves', [fen]).then((json) => ({
		data: withNumericKeys({}, JSON.parse(json))
	}))

export const getBestMove = ({ fen }) =>
	call('getBestMove', [fen]).then((data) => ({ data }))

export const isGameOver = ({ fen }) =>
	call('isGameOver', [fen]).then((data) => ({ data }))

export const preload = () => getWorker()
