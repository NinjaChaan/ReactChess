import React, { useState, useEffect, useCallback } from 'react'
import styled from 'styled-components'
import EventSource from 'eventsource'
import Square from '../Components/Square'
import SettingsModule from '../Components/SettingsModule'
import Movehistory from '../Components/Movehistory'
import Ellipsis from './Ellipsis'
import { CoordinatesNumbers, CoordinatesLetters } from '../Components/Coordinates'
import FENBoard from 'fen-chess-board'
import { device } from '../devices'
import axios from 'axios'
import { v4 as uuidv4 } from 'uuid'
import w_q from '../../resources/chesspieces/Chess_qlt45.svg'
import b_q from '../../resources/chesspieces/Chess_qdt45.svg'
import w_k from '../../resources/chesspieces/Chess_klt45.svg'
import b_k from '../../resources/chesspieces/Chess_kdt45.svg'
import w_n from '../../resources/chesspieces/Chess_nlt45.svg'
import b_n from '../../resources/chesspieces/Chess_ndt45.svg'
import w_r from '../../resources/chesspieces/Chess_rlt45.svg'
import b_r from '../../resources/chesspieces/Chess_rdt45.svg'
import w_b from '../../resources/chesspieces/Chess_blt45.svg'
import b_b from '../../resources/chesspieces/Chess_bdt45.svg'
import w_p from '../../resources/chesspieces/Chess_plt45.svg'
import b_p from '../../resources/chesspieces/Chess_pdt45.svg'

const Container = styled.div`
	display: flex;
`

const BoardBorder = styled.div`
	background-color: sienna;
	padding: 20px;
	border-radius: 4px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	height: 410px;
`

const BoardStyle = styled.div`
	width: fit-content;
	height: fit-content;
	display: grid;
	grid-template-columns: repeat(8,1fr);
	border-radius: 4px;
	border: 5px solid peru;
	border-radius: 4px;
	transform: ${(props) => (props.playAs === 1 && `rotate(180deg)`)};
`

const BoardContainer = styled.div`
	width: 100%;
	display: flex;
	flex-direction: column;
    flex-wrap: wrap;

	@media ${device.tablet} {
    	flex-direction: row;
  	}
`

const StartContainer = styled.div`
	background-color: burlywood;
	border-radius: 4px;
	/* width: 90%; */
    height: max-content;
    margin: 10% 15%;
    padding: 5px;
`
const StartContainerInner = styled.div`
	background-color: sienna;
	border-radius: 4px;
	height: calc(100% - 10px);
	padding: 5px;
`

const ButtonGroup = styled.div`
	display: flex;
	justify-content: space-around;
	width: 80%;
	margin: 10px auto;
`

const Line = styled.hr`
	border-radius: 4px;
	background-image: linear-gradient(to right,rgba(0,0,0,0),rgb(241, 210, 171.75),rgba(0,0,0,0));
	height: 1px;
	border: none;
`

const StartButton = styled.button`
	height: 40px;
	/* position: absolute; */
	/* top: 200px; */
	/* left: ${(props) => (props.width && `calc(50% - ${props.width}px/ 2 ${props.offset})`)}; */
	border: none;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	background-color: ${(props) => (props.white && 'white')
		|| 'black'
	};
	color: ${(props) => (props.white && 'black')
		|| 'white'
	};
	border-radius: 4px;
	font-weight: 600;
	&:hover{
		background-color:#fffafac7;
	}
`

const ColumnContainer = styled.div`
	width: 100%;
	display: flex;
	flex-direction: column;
    flex-wrap: wrap;
	align-items: center;
`

const SettingsButton = styled.button`
	background-color: sienna;
	background-color: ${(props) => (props.background === 'white' && 'white')
		|| (props.background === 'black' && 'black')
	};
	min-width: 60px;
	width: fit-content;
	height: 35px;
	@media ${device.tablet}{
		width: max-content;
		height: 30px;
	}
	border-radius: 4px;
	color: #f1d2ab;
	color: ${(props) => (props.background === 'white' && 'black')
		|| (props.background === 'black' && 'white')
	};
	border: 2px solid burlywood;
	margin-right: 10px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	outline: none;
	&:hover {
		background-color: ${(props) => (props.background === 'white' && 'rgb(220,220,220)')
		|| (props.background === 'black' && 'rgb(80,80,80)')
		|| 'sienna'
	};
		opacity: 100%;
	};
	/* &:last-of-type{
		margin-right: 0px;
	} */
	&:active {
		transform: translateY(2px);
	}
`

const DrawAnswerButton = styled(SettingsButton)`
	color: black;
	border: 2px solid rgba(0,0,0,0.75);
	font-weight: bold;
`

const MatchmakingButton = styled(SettingsButton)`
	width: 50% !important;
	margin-bottom: 10px;
`

const GameIdInput = styled.input`
	width: 90%;
	background-color: sienna;
    border-radius: 4px;
    border: 2px solid burlywood;
    color: #f1d2ab;
    outline: none;
	margin-bottom: 10px;
`

const Mask = styled.div`
	width: 450px;
	height: 450px;
	position: absolute;
	background-color: #615f5fc7;
	display: flex;
	border-radius: 4px;
	margin-left: -21px;
	flex-direction: column;
`

const Banner = styled.div`
	margin: 160px auto 0 auto;	
	margin-top: ${(props) => (props.topMargin + 'px')};
	background-color: ${(props) => (props.winner === "w" && '#c1fc90')
		|| '#fc9590'
	};
	display: grid;
	border-radius: 4px;
	padding: 10px;
	width: 80%;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	height: fit-content;
	z-index: 1;
`

const BannerText = styled.span`
	margin: auto;
	font-weight: 600;
`

const ThinkingText = styled.span`
	display: block;
	height: 22px;
	width: 450px;
	font-size: larger;
	text-align: center;
	color: white;
	margin-bottom: 5px;
`

const PromotionContainer = styled.div`
	position: relative;
	top: 30%;
	left: 115px;
	background-color: burlywood;
	height: 90px;
	border-radius: 4px;
	padding: 5px;

`

const PromotionInner = styled.div`
	background-color: sienna;
	padding: 5px;
`

const PromotionTitle = styled.span`
    text-align: center;
    width: 100%;
    display: block;
	font-size: ${(props) => (props.size && `${props.size}px`)
		|| 'larger'
	};
    margin-bottom: 5px;
	color: #f1d2ab;
`

const PromotionButton = styled.button`
	background: ${(props) => (props.image && `url(${props.image})`)};
	background-color: beige;
	height: 50px;
	width: 50px;
	border-radius: 4px;
`

const pieceSwitch = (piece, size) => {
	switch (piece) {
		case 'k':
			return (<img width={size} height={size} src={b_k}></img>)
		case 'K':
			return (<img width={size} height={size} src={w_k}></img>)
		case 'q':
			return (<img width={size} height={size} src={b_q}></img>)
		case 'Q':
			return (<img width={size} height={size} src={w_q}></img>)
		case 'n':
			return (<img width={size} height={size} src={b_n}></img>)
		case 'N':
			return (<img width={size} height={size} src={w_n}></img>)
		case 'r':
			return (<img width={size} height={size} src={b_r}></img>)
		case 'R':
			return (<img width={size} height={size} src={w_r}></img>)
		case 'b':
			return (<img width={size} height={size} src={b_b}></img>)
		case 'B':
			return (<img width={size} height={size} src={w_b}></img>)
		case 'p':
			return (<img width={size} height={size} src={b_p}></img>)
		case 'P':
			return (<img width={size} height={size} src={w_p}></img>)
		case '':
			return (null)

		default:
			return (null)
	}
}

const playTurn = (fen) => {
	const request = axios.post(`/playTurn`, fen)
	return request.then((response) => response).catch((error) => (error.response))
}

const getLegalMoves = (fen) => {
	const request = axios.post(`/getLegalMoves`, fen)
	return request.then((response) => response).catch((error) => (error.response))
}

const getBestMove = (fen) => {
	const request = axios.post(`/getBestMove`, fen)
	return request.then((response) => response).catch((error) => (error.response))
}

const startPVP = (pguid) => {
	const request = axios.post(`/startPVP`, { playerId: pguid, color: 'white' })
	return request.then((response) => response).catch((error) => (error.response))
}

const joinRandomPVP = (pguid) => {
	const request = axios.post(`/joinRandomPVP`, { playerId: pguid })
	return request.then((response) => response).catch((error) => (error.response))
}



const Board = () => {
	const [difficulty, setDifficulty] = useState(1)
	const [running, setRunning] = useState(false)
	const [winner, setWinner] = useState(null)
	const [winnerText, setWinnerText] = useState()
	const [turn, setTurn] = useState(0)
	const [playAs, setPlayAs] = useState(0)
	const [fen, setFen] = useState(`${new FENBoard("start").fen}`)
	const [fenExtras, setFenExtras] = useState({ toMove: 'w', castling: 'KQkq' })
	const [items, setItems] = useState([])
	const [startWidth, setStartWidth] = useState()
	const [fenHistory, setFenHistory] = useState({})
	const [moveHistory, setMoveHistory] = useState([])
	const [legalMoves, setLegalMoves] = useState([])
	const [lastMove, setLastMove] = useState("")
	const [hintMove, setHintMove] = useState("")
	const [canAskForHint, setCanAskForHint] = useState(true)
	const [showOnlyStart, setShowOnlyStart] = useState(false)
	const [selectedPiece, setSelectedPiece] = useState({})
	const [allowedMoves, setAllowedMoves] = useState([])
	const [botThinking, setBotThinking] = useState(false)
	const [promotion, setPromotion] = useState(false)
	const [toPromote, setToPromote] = useState({})
	const [enPassant, setEnPassant] = useState('-')
	const [showOptions, setShowOptions] = useState(window.matchMedia(device.laptop).matches)
	const [showPVPOptions, setShowPVPOptions] = useState(false)
	const [gameId, setGameId] = useState("")
	const [playerId, setPlayerId] = useState("")
	const [message, setMessage] = useState("")
	const [pvpRunning, setPVPRunning] = useState(false)
	const [moved, setMoved] = useState(false)
	const [data, setData] = useState(null)
	const [matches, setMatches] = useState(0)
	const [playersMatchmaking, setPlayersMatchmaking] = useState(0)
	const [offer, setOffer] = useState(-1)

	const playPVPTurn = (fen) => {
		const request = axios.post(`/pvp/move/${gameId}/${playerId}`, { gameId, playerId, fen, move: lastMove })
		return request.then((response) => response).catch((error) => (error.response))
	}

	const getMatchesInfo = () => {
		const request = axios.get(`/matches`)
		request.then((response) => {
			console.log('matches', response.data)
			setMatches(response.data)
		}).catch((error) => (error.response))
		const request2 = axios.get(`/matchmaking`)
		request2.then((response) => {
			console.log('matchmaking', response.data)
			setPlayersMatchmaking(response.data)
		}).catch((error) => (error.response))
	}

	useEffect(() => {
		getMatchesInfo()
	}, []);

	useEffect(() => {
		if (data) {
			var splitfen = data.fen.split(' ')

			if (data.fen.includes("wins")) {
				setRunning(false)
				setWinnerText(data.fen)
				if (data.fen.includes("White")) {
					setWinner("w")
				} else {
					setWinner("b")
				}
				doFinalMove(result) //TODO: check
			} else if (data.fen.includes("Draw")) {
				setRunning(false)
				setWinnerText(data.fen)
				setWinner("d")
				doFinalMove(result)//check
			} else {
				if (data.fen in fenHistory) {
					fenHistory[data.fen]++
					if (fenHistory[data.fen] === 3) {
						setRunning(false)
						setWinnerText("Draw")
						setWinner("d")
					}
				} else {
					fenHistory[data.fen] = 1
				}
				if (data.lastMove !== '' && lastMove !== data.lastMove) {
					setLastMove(data.lastMove)
					moveHistory.push(
						{
							move: data.lastMove,
							color: splitfen[1] === "b" ? "w" : "b"
						})
				}
				if ('legalMoves' in data) {
					setLegalMoves(Object.values(data.legalMoves))
				} else {
					setLegalMoves([])
				}

				setTurn(splitfen[1] === "b" ? 1 : 0)
				setTimeout(() => {
					updateScroll()
				}, 100)
				// fenBoard.fen = result.data.fen
				setFen(splitfen[0])
				setFenExtras({ toMove: splitfen[1], castling: splitfen[2] || '' })
				setEnPassant(data.fen[data.fen.length - 1] !== '-' ? data.fen.substring(data.fen.length - 2) : '-')
			}
		}
	}, [data]);

	const startStream = (guid, pguid) => {
		console.log('Connecting to event stream')
		const eventSourceInitDict = {
			headers: {

			}
		}
		const eventSource = new EventSource(`/pvp/${guid}/${pguid}`)
		eventSource.onopen = (m) => {
			console.log('Connected!', m)
		}
		eventSource.onerror = (e) => console.log(e)
		eventSource.onmessage = (e) => {
			const data = JSON.parse(e.data)
			console.log('stream data', data)

			if (!(typeof (data) !== 'string' && 'fen' in data && 'lastMove' in data)) {
				setMessage(data)

				// if (typeof (data) === 'string') {
				// 	if (data.includes("wins")) {
				// 		setRunning(false)
				// 		setWinnerText(data)
				// 		if (data.includes("White")) {
				// 			setWinner("w")
				// 		} else {
				// 			setWinner("b")
				// 		}

				// 		setPVPRunning(false)
				// 		eventSource.close()
				// 		return
				// 	} else if (data.includes("Draw")) {
				// 		setRunning(false)
				// 		setWinnerText(data)
				// 		setWinner("d")
				// 		setPVPRunning(false)
				// 		eventSource.close()
				// 		return
				// 	}else if (data.includes("resigned")) {
				// 		setRunning(false)
				// 		setWinnerText(data)
				// 		if (data.includes("White")) {
				// 			setWinner("b")
				// 		} else {
				// 			setWinner("w")
				// 		}

				// 		setPVPRunning(false)
				// 		eventSource.close()
				// 		return
				// 	}
				// }

				return
			}


			setPVPRunning(true)
			setShowPVPOptions(true)
			setMessage("")

			setData(data)

			if ('status' in data) {
				status = data['status']
				if (!status.includes("ongoing")) {
					setMessage(status)
				}
				if (status.includes("wins")) {
					setRunning(false)
					setWinnerText(status)
					if (status.includes("White")) {
						setWinner("w")
					} else {
						setWinner("b")
					}

					setPVPRunning(false)
					eventSource.close()
					return
				} else if (status.includes("Draw")) {
					setRunning(false)
					setWinnerText(status)
					setWinner("d")
					setPVPRunning(false)
					eventSource.close()
					return
				} else if (status.includes("resigned")) {
					setRunning(false)
					setWinnerText(status)
					if (status.includes("White")) {
						setWinner("b")
					} else {
						setWinner("w")
					}

					setPVPRunning(false)
					eventSource.close()
					return
				} else if (status.includes("offer")) {
					if (status.includes("white")) {
						setOffer(0)
						setWinnerText('white offers draw')
						setWinner("w")
						console.log('white offers draw')
					} else {
						setOffer(1)
						setWinnerText('black offers draw')
						setWinner("w")
						console.log('black offers draw')
					}
					if (offer != playAs) {
						setMessage("Opponent is considering your draw offer")
					}
					return
				} else if (status.includes("reject")) {
					setOffer(-1)
					return
				}
			}

			return () => {
				eventSource.close()
				console.log('eventSource closed!')
			}
		}
	}

	const startGame = (white) => {
		setWinner(null)
		setRunning(true)
		setFenHistory({})
		setFen(`${new FENBoard("start").fen}`)
		setFenExtras({ toMove: 'w', castling: 'KQkq' })
		setMoveHistory([])
		setLastMove("")
		setPlayAs(white ? 0 : 1)
		if (white === true) {
			setTurn(0)
			getLegalMoves({ fen: `${new FENBoard("start").fen} w KQkq` })
				.then((result) => {
					console.log('fen result', result)
					let moves = []
					for (const move in result.data) {
						if (!isNaN(move)) {
							moves.push(result.data[move])
						}
					}
					setLegalMoves(moves)
				})
		} else {
			setTurn(0)
		}
	}

	useEffect(() => {
		if (document.getElementById('start')) {
			setStartWidth(document.getElementById('start').scrollWidth)
		}
	}, [winner]);

	const squareInLastMove = (i, j, r1c, r1r, r2c, r2r, onlyStart = false) => {
		if (onlyStart) {
			return (j === r1c && i === r1r)
		}
		return (j === r1c && i === r1r) || (j === r2c && i === r2r)
	}

	const changeTurnAndCastling = ({ castle = false, king = false, qtower = false, ktower = false }) => {
		const nextToMove = fenExtras.toMove === 'w' ? 'b' : 'w'
		if (castle || king) {
			let extras = fenExtras.castling.replace(fenExtras.toMove === 'w' ? "K" : "k", "")
			extras = extras.replace(fenExtras.toMove === 'w' ? "Q" : "q", "")
			setFenExtras({ toMove: nextToMove, castling: extras })
		} else if (qtower) {
			let extras = fenExtras.castling.replace(fenExtras.toMove === 'w' ? "Q" : "q", "")
			setFenExtras({ toMove: nextToMove, castling: extras })
		} else if (ktower) {
			let extras = fenExtras.castling.replace(fenExtras.toMove === 'w' ? "K" : "k", "")
			setFenExtras({ toMove: nextToMove, castling: extras })
		} else {
			setFenExtras({ toMove: nextToMove, castling: fenExtras.castling })
		}
	}

	const promote = (selection) => {
		const fenBoard = new FENBoard(fen)
		fenBoard.move(`${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}`, `${String.fromCharCode(toPromote.x + 97)}${8 - toPromote.y}`)
		fenBoard.board[toPromote.y][toPromote.x] = selection
		setFen(fenBoard.fen)
		const move = `${((fenBoard.board[toPromote.y][toPromote.x] !== 'P' && fenBoard.board[toPromote.y][toPromote.x] !== 'p') ? fenBoard.board[toPromote.y][toPromote.x] : '')}${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}-${String.fromCharCode(toPromote.x + 97)}${8 - toPromote.y}`
		setLastMove(move)
		setTurn(1)
		setFen(fenBoard.fen)
		setSelectedPiece({})
		setAllowedMoves([])
		moveHistory.push(
			{
				move,
				color: fenExtras.toMove
			})
		setPromotion(false)
		setToPromote({})
		changeTurnAndCastling({})
	}

	const clickSquare = (coords) => {
		const fenBoard = new FENBoard(fen)
		const pieceSide = fenBoard.board[coords.y][coords.x] === fenBoard.board[coords.y][coords.x].toUpperCase() ? 0 : 1

		if (fenBoard.board[coords.y][coords.x] !== "" && pieceSide === playAs && playAs === turn) {
			let newAllowedMoves = []
			if (selectedPiece.x === coords.x && selectedPiece.y === coords.y) {
				setSelectedPiece({})
				setAllowedMoves([])
				return
			}
			setSelectedPiece(coords)
			for (let i = 0; i < legalMoves.length; i++) {
				let castling = false;
				const regex = RegExp('B|R|N|K|Q')

				const cleanedMove = (regex.test(legalMoves[i][0]) ? legalMoves[i].substring(1) : legalMoves[i]) || "öööööö"
				if (legalMoves[i] === "O-O") {
					if (fenExtras.toMove === 'w' && coords.x === 4 && coords.y === 7) {
						newAllowedMoves.push(`O-O`)
					} else if (fenExtras.toMove === 'b' && coords.x === 4 && coords.y === 0) {
						newAllowedMoves.push(`O-O`)
					}
				}
				else if (legalMoves[i] === "O-O-O") {
					if (fenExtras.toMove === 'w' && coords.x === 4 && coords.y === 7) {
						newAllowedMoves.push(`O-O-O`)
					} else if (fenExtras.toMove === 'b' && coords.x === 4 && coords.y === 0) {
						newAllowedMoves.push(`O-O-O`)
					}
				} else {
					const r1c = cleanedMove[0].charCodeAt(0) - 'a'.charCodeAt(0);
					const r1r = 7 - (cleanedMove[1].charCodeAt(0) - '1'.charCodeAt(0));

					if (r1c === coords.x && r1r === coords.y) {
						const r2c = cleanedMove[3].charCodeAt(0) - 'a'.charCodeAt(0);
						const r2r = 7 - (cleanedMove[4].charCodeAt(0) - '1'.charCodeAt(0));
						newAllowedMoves.push(`${r2c}${cleanedMove[2]}${r2r}`)
					}
				}

			}
			setAllowedMoves(newAllowedMoves)
		}
		let moved = false
		let move = ''
		if (allowedMoves.includes(`${coords.x}-${coords.y}`) || allowedMoves.includes(`${coords.x}x${coords.y}`)) {
			let dash = '-'
			if (fenBoard.board[coords.y][coords.x] !== '') {
				dash = 'x'
			}
			fenBoard.move(`${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}`, `${String.fromCharCode(coords.x + 97)}${8 - coords.y}`)
			if (fenExtras.toMove === 'w' && coords.y === 0 && (fenBoard.board[coords.y][coords.x] === 'P' || fenBoard.board[coords.y][coords.x] === 'p')) {
				setPromotion(true)
				setToPromote(coords)
			} else {
				moved = true
				if (allowedMoves.includes(`${coords.x}x${coords.y}`) && enPassant === `${String.fromCharCode(coords.x + 97)}${8 - coords.y}`) {
					console.log(`en passant at ${String.fromCharCode(coords.x + 97)}${8 - coords.y + (fenExtras.toMove === 'w' ? -1 : 1)}`)
					fenBoard.put(`${String.fromCharCode(coords.x + 97)}${8 - coords.y + (fenExtras.toMove === 'w' ? -1 : 1)}`, "");
					dash = 'x'
				}
				move = `${((fenBoard.board[coords.y][coords.x] !== 'P' && fenBoard.board[coords.y][coords.x] !== 'p') ? fenBoard.board[coords.y][coords.x].toUpperCase() : '')}${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}${dash}${String.fromCharCode(coords.x + 97)}${8 - coords.y}`

				if ((fenBoard.board[coords.y][coords.x] === 'P' || fenBoard.board[coords.y][coords.x] === 'p') && Math.abs(coords.y - selectedPiece.y) === 2) {
					console.log('en passant possible')
					setEnPassant(`${String.fromCharCode(coords.x + 97)}${fenExtras.toMove === 'w' ? '3' : '6'}`)
				} else {
					setEnPassant('-')
				}
			}
		} if (allowedMoves.includes(`O-O`)) {
			if (fenExtras.toMove === 'w' && coords.x === 6 && coords.y === 7) {
				fenBoard.move(`e1`, `g1`);
				fenBoard.move(`h1`, `f1`);
				moved = true
				move = 'O-O'
			} else if (fenExtras.toMove === 'b' && coords.x === 6 && coords.y === 0) {
				fenBoard.move(`e8`, `g8`);
				fenBoard.move(`h8`, `f8`);
				moved = true
				move = 'O-O'
			}
		} if (allowedMoves.includes(`O-O-O`)) {
			if (fenExtras.toMove === 'w' && coords.x === 2 && coords.y === 7) {
				fenBoard.move(`e1`, `c1`);
				fenBoard.move(`a1`, `d1`);
				moved = true
				move = 'O-O-O'
			} else if (fenExtras.toMove === 'b' && coords.x === 2 && coords.y === 0) {
				fenBoard.move(`e8`, `c8`);
				fenBoard.move(`a8`, `d8`);
				moved = true
				move = 'O-O-O'
			}
		}

		if (moved) {
			setHintMove("")
			setLastMove(move)
			setTurn(turn === 0 ? 1 : 0)
			setFen(fenBoard.fen)
			setSelectedPiece({})
			setAllowedMoves([])

			if (move === 'O-O' || move === 'O-O-O') {
				changeTurnAndCastling({ castle: true })
			} else if (move[0] === 'K') {
				changeTurnAndCastling({ king: true })
			} else if (move[0] === 'R' && move[1] === 'h' && move[2] === '1') {
				changeTurnAndCastling({ ktower: true })
			} else if (move[0] === 'R' && move[1] === 'a' && move[2] === '1') {
				changeTurnAndCastling({ qtower: true })
			} else {
				changeTurnAndCastling({})
			}

			moveHistory.push(
				{
					move,
					color: fenExtras.toMove
				})
		}
		setMoved(true)

		return (pieceSide === playAs && playAs === turn)
	}

	const checkHint = (i, j) => {
		const regex = RegExp('B|R|N|K|Q')
		const cleanedMove = (regex.test(hintMove[0]) ? hintMove.substring(1) : hintMove) || "öööööö"
		let showMove = true
		if (hintMove === "O-O") {
			const h1c = 4;
			const h1r = fenExtras.toMove === 'w' ? 7 : 0;
			const h2c = 6;
			const h2r = fenExtras.toMove === 'w' ? 7 : 0;

			return hintMove !== "" ? squareInLastMove(i, j, h1c, h1r, h2c, h2r) : false
		} else if (hintMove === "O-O-O") {
			const h1c = 4;
			const h1r = fenExtras.toMove === 'w' ? 7 : 0;
			const h2c = 2;
			const h2r = fenExtras.toMove === 'w' ? 7 : 0;

			return hintMove !== "" ? squareInLastMove(i, j, h1c, h1r, h2c, h2r) : false

		} else {
			const h1c = showMove ? cleanedMove[0].charCodeAt(0) - 'a'.charCodeAt(0) : "";
			const h1r = showMove ? 7 - (cleanedMove[1].charCodeAt(0) - '1'.charCodeAt(0)) : "";
			const h2c = showMove ? cleanedMove[3].charCodeAt(0) - 'a'.charCodeAt(0) : "";
			const h2r = showMove ? 7 - (cleanedMove[4].charCodeAt(0) - '1'.charCodeAt(0)) : "";
			return hintMove !== "" ? squareInLastMove(i, j, h1c, h1r, h2c, h2r, showOnlyStart) : false
		}
	}

	const setBoard = (movedPiece) => {
		const fenBoard = new FENBoard(fen)
		let showMove = true;

		const regex = RegExp('B|R|N|K|Q')

		const cleanedMove = (regex.test(lastMove[0]) ? lastMove.substring(1) : lastMove) || "öööööö"

		if (lastMove === "O-O" || lastMove === "O-O-O") {
			showMove = false
		}
		const r1c = showMove ? cleanedMove[0].charCodeAt(0) - 'a'.charCodeAt(0) : "";
		const r1r = showMove ? 7 - (cleanedMove[1].charCodeAt(0) - '1'.charCodeAt(0)) : "";
		const r2c = showMove ? cleanedMove[3].charCodeAt(0) - 'a'.charCodeAt(0) : "";
		const r2r = showMove ? 7 - (cleanedMove[4].charCodeAt(0) - '1'.charCodeAt(0)) : "";

		const tempItems = []
		for (let i = 0; i < 8; i++) {
			for (let j = 0; j < 8; j++) {
				const moved = showMove ? squareInLastMove(i, j, r1c, r1r, r2c, r2r) : false
				const shortCastle = allowedMoves.includes("O-O") && ((turn === 0 && j === 6 && i === 7) || (turn === 1 && j === 6 && i === 0))
				const longCastle = allowedMoves.includes("O-O-O") && ((turn === 0 && j === 2 && i === 7) || (turn === 1 && j === 2 && i === 0))
				const allowed = !movedPiece && (allowedMoves.includes(`${j}-${i}`) || allowedMoves.includes(`${j}x${i}`) || shortCastle || longCastle)
				const showHint = checkHint(i, j)
				// console.log('show hint', showHint)

				tempItems.push(
					<Square
						id={`${j}${i}`}
						key={`${j}-${i}`}
						allowedMoves={allowedMoves.length}
						white={(i * 7 + j) % 2 === 0}
						allowed={allowed}
						hint={showHint}
						coords={{ x: j, y: i }}
						clickCallback={clickSquare}
						moved={moved}
						playAs={playAs}>
						{pieceSwitch(fenBoard.board[i][j], 46)}
					</Square>
				)
			}
		}
		setItems(tempItems)
	}

	const updateScroll = () => {
		var element = document.getElementById("moveContainer");
		element.scrollTop = element.scrollHeight;
	}

	const doFinalMove = (result) => {
		if (result.data.lastMove !== '') {
			const fenBoard = new FENBoard(fen)
			var splitfen = result.data.fen.split(' ')
			moveHistory.push(
				{
					move: result.data.lastMove,
					color: splitfen[1] === "b" ? "w" : "b"
				})
			fenBoard.move(`${result.data.lastMove[0]}${result.data.lastMove[1]}`, `${result.data.lastMove[3]}${result.data.lastMove[4]}`);
			setFen(fenBoard.fen)
			setBoard(true)
		}
	}

	const playBotTurn = (fen) => {
		console.log('play fen', fen)
		setBotThinking(true)
		setShowOnlyStart(false)
		if (fen.length > 0) {
			playTurn({ fen, difficulty })
				.then((result) => {
					console.log('result', result)
					console.log(`receive fen ${result.data.fen} ${result.data.enPassant}`)
					var splitfen = result.data.fen.split(' ')
					if (result.data.fen.includes("wins")) {
						setRunning(false)
						setMessage(result.data.fen)
						if (result.data.fen.includes("White")) {
							setWinner("w")
						} else {
							setWinner("b")
						}
						setBotThinking(false)
						doFinalMove(result)
					} else if (result.data.fen.includes("Draw")) {
						setRunning(false)
						setMessage(result.data.fen)
						setWinner("d")
						setBotThinking(false)
						doFinalMove(result)
					} else {
						if (result.data.fen in fenHistory) {
							fenHistory[result.data.fen]++
							if (fenHistory[result.data.fen] === 3) {
								setRunning(false)
								setMessage("Draw")
								setWinner("d")
							}
						} else {
							fenHistory[result.data.fen] = 1
						}
						moveHistory.push(
							{
								move: result.data.lastMove,
								color: splitfen[1] === "b" ? "w" : "b"
							})
						let moves = []
						for (const move in result.data) {
							if (!isNaN(move)) {
								moves.push(result.data[move])
							}
						}
						setLegalMoves(moves)
						setTurn(splitfen[1] === "b" ? 1 : 0)
						setLastMove(result.data.lastMove)
						setTimeout(() => {
							updateScroll()
						}, 100)
						// fenBoard.fen = result.data.fen
						setFen(splitfen[0])
						setFenExtras({ toMove: splitfen[1], castling: splitfen[2] || '' })
						setEnPassant(result.data.enPassant)
						setBotThinking(false)
					}
				})
		}
	}

	useEffect(() => {
		setBoard(false)
		if (((playAs === 0 && turn === 1) || (playAs === 1 && turn === 0)) && !botThinking && running) {
			const castling = fenExtras.castling !== '' ? ' ' + fenExtras.castling : ''
			playBotTurn(`${fen} ${fenExtras.toMove}${castling}${' ' + enPassant}`)
			setShowOnlyStart(false)
		}
		else if (((playAs === 0 && turn === 1) || (playAs === 1 && turn === 0)) && moved && pvpRunning) {
			console.log('lastmove at send', lastMove)
			const castling = fenExtras.castling !== '' ? ' ' + fenExtras.castling : ''
			playPVPTurn(`${fen} ${fenExtras.toMove}${castling}${' ' + enPassant}`)
			setShowOnlyStart(false)
			setMoved(false)
		}
	}, [fen, fenExtras, allowedMoves, selectedPiece, legalMoves, botThinking, turn, promotion, hintMove, showOnlyStart, playAs, lastMove])

	useEffect(() => {
		document.addEventListener("touchmove", function (e) { e.preventDefault() });
		return () => {
			document.removeEventListener("touchmove", function (e) { e.preventDefault() });
		};
	});

	const getHint = (onlyStart) => {
		if (canAskForHint) {
			setCanAskForHint(false)
			setShowOnlyStart(onlyStart)
			if (fen.length > 0) {
				const castling = fenExtras.castling !== '' ? ' ' + fenExtras.castling : ''
				getBestMove({ fen: `${fen} ${fenExtras.toMove}${castling}${' ' + enPassant}` })
					.then((result) => {
						setHintMove(result.data)
						setCanAskForHint(true)
					})
			}
		}
	}

	const startNewPVP = () => {
		console.log('start pvp??')
		const pguid = uuidv4()
		setPlayerId(pguid)
		startPVP(pguid)
			.then((result) => {
				console.log('result', result)
				startStream(result.data.gameId, pguid)
				setGameId(result.data.gameId)
			})
	}

	const joinPVP = () => {
		const gameId = document.getElementById("gameid").value
		const pguid = uuidv4()
		setPlayerId(pguid)
		setGameId(gameId)
		startStream(gameId, pguid)
		setPlayAs(1)
	}

	const joinPVPRand = () => {
		const pguid = uuidv4()
		setPlayerId(pguid)
		joinRandomPVP(pguid).then((result) => {
			setGameId(result.data.gameId)
			startStream(result.data.gameId, pguid)
			setPlayAs(1)
		})
	}

	const resign = () => {
		const request = axios.post(`/pvp/resign/${gameId}/${playerId}`, { gameId, playerId })
		return request.then((response) => response).catch((error) => (error.response))
	}

	const offerDraw = () => {
		setOffer(-1)
		const request = axios.post(`/pvp/offerdraw/${gameId}/${playerId}`, { gameId, playerId })
		return request.then((response) => response).catch((error) => (error.response))
	}

	const acceptDraw = () => {
		offerDraw()
	}

	const rejectDraw = () => {
		setOffer(-1)
		const request = axios.post(`/pvp/rejectdraw/${gameId}/${playerId}`, { gameId, playerId })
		return request.then((response) => response).catch((error) => (error.response))
	}

	return (
		<>
			<div style={{ display: 'flex', width: '100%', marginLeft: '2.5px' }}>
				<BoardContainer>
					{window.matchMedia(device.laptop).matches && (
						<SettingsModule
							showOptions={showOptions}
							difficulty={difficulty}
							setDifficulty={setDifficulty}
							getHint={getHint}
							setShowOptions={setShowOptions}
							showPVPOptions={showPVPOptions}
							setShowPVPOptions={setShowPVPOptions}
							startPVP={startNewPVP}
							joinPVP={joinPVP}
							resign={resign}
							draw={offerDraw}
							pvpRunning={pvpRunning}
							playerId={playerId}
							gameId={gameId}
							showOptions={showOptions} />
					)}
					<div style={{ marginLeft: '25px' }}>
						<ThinkingText>
							{botThinking ? (
								<>
									<span>The bot is thinking</span>
									<Ellipsis />
								</>
							) : (
									<span> </span>
								)}
						</ThinkingText>
						<Container>
							<CoordinatesLetters playAs={playAs} />
							<CoordinatesNumbers playAs={playAs} />
							<BoardBorder>
								<BoardStyle playAs={playAs}>
									{items}
								</BoardStyle>
							</BoardBorder>
							<CoordinatesNumbers playAs={playAs} />
							{window.matchMedia(device.laptop).matches && (
								<Movehistory moveHistory={moveHistory} />
							)}
							{((!running && !pvpRunning) || offer != -1) && (
								<Mask>
									{message !== "" && (
										<Banner
											topMargin={(!running && !pvpRunning) ? 20 : 160}
											winner={winner}>
											<BannerText>{message}</BannerText>
										</Banner>
									)}
									{offer != -1 && offer != playAs && (
										<ButtonGroup>
											<DrawAnswerButton
												id="start"
												style={{ backgroundColor: "#c1fc90" }}
												onClick={acceptDraw}>
												Accept
											</DrawAnswerButton>
											<DrawAnswerButton
												id="start"
												style={{ backgroundColor: "#fc9590" }}
												onClick={rejectDraw}>
												Reject
											</DrawAnswerButton>
										</ButtonGroup>
									)}
									{!playerId && (
										<StartContainer>
											<StartContainerInner>
												<div>
													<PromotionTitle>Play against a bot</PromotionTitle>
													<ButtonGroup>
														<SettingsButton
															id="start"
															background="white"
															onClick={() => { startGame(true) }}>
															Play as white
												</SettingsButton>
														<SettingsButton
															id="start"
															background="black"
															onClick={() => { startGame(false) }}>
															Play as black
												</SettingsButton>
													</ButtonGroup>
												</div>
												<Line />
												<PromotionTitle>Play against a human</PromotionTitle>
												{/* <PromotionTitle size="14">{`Matches running: ${matches}`}</PromotionTitle> */}
												<PromotionTitle size="14">{`Players looking for a match: ${playersMatchmaking}`}</PromotionTitle>
												<ColumnContainer>
													<MatchmakingButton onClick={startNewPVP}>Start a match</MatchmakingButton>
													<MatchmakingButton onClick={joinPVPRand}>Find a match</MatchmakingButton>
												</ColumnContainer>
												<PromotionTitle size="16">Join a match with id</PromotionTitle>
												<ColumnContainer>
													<GameIdInput id="gameid"></GameIdInput>
													<MatchmakingButton onClick={joinPVP}>Join</MatchmakingButton>
												</ColumnContainer>
											</StartContainerInner>
										</StartContainer>
									)}
								</Mask>
							)}
							<CoordinatesLetters offsetTop={430} playAs={playAs} />
							{promotion && (
								<Mask>
									<PromotionContainer>
										<PromotionInner>
											<PromotionTitle>What to promote to?</PromotionTitle>
											<div>
												<PromotionButton image={w_q} onClick={() => promote('Q')}></PromotionButton>
												<PromotionButton image={w_r} onClick={() => promote('R')}></PromotionButton>
												<PromotionButton image={w_b} onClick={() => promote('B')}></PromotionButton>
												<PromotionButton image={w_n} onClick={() => promote('N')}></PromotionButton>
											</div>
										</PromotionInner>
									</PromotionContainer>
								</Mask>
							)}
						</Container>
					</div>
					{window.matchMedia(device.tabletMAX).matches && (
						<SettingsModule
							mobile
							showOptions={showOptions}
							difficulty={difficulty}
							setDifficulty={setDifficulty}
							getHint={getHint}
							setShowOptions={setShowOptions}
							showPVPOptions={showPVPOptions}
							setShowPVPOptions={setShowPVPOptions}
							startPVP={startNewPVP}
							joinPVP={joinPVP}
							resign={resign}
							draw={offerDraw}
							pvpRunning={pvpRunning}
							playerId={playerId}
							gameId={gameId}
							showOptions={showOptions} />
					)}
					{window.matchMedia(device.tabletMAX).matches && (
						<Movehistory moveHistory={moveHistory} mobile={true} />
					)}
				</BoardContainer>
			</div>
		</>
	)
}

export default Board
