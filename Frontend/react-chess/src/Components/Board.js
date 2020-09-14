import React, { useState, useEffect, useCallback } from 'react'
import styled from 'styled-components'
import EventSourcePoly from 'eventsource'
import Square from '../Components/Square'
import Ellipsis from './Ellipsis'
import { CoordinatesNumbers, CoordinatesLetters } from '../Components/Coordinates'
import FENBoard from 'fen-chess-board'
import axios from 'axios'
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
`

const BoardStyle = styled.div`
	width: fit-content;
	height: fit-content;
	display: grid;
	grid-template-columns: repeat(8,1fr);
	border-radius: 4px;
	border: 5px solid peru;
	border-radius: 4px;
`
const BoardContainer = styled.div`
	margin: 0 auto;
`

const StartButton = styled.button`
	height: 40px;
	position: absolute;
	top: 200px;
	left: ${(props) => (props.width && `calc(50% - ${props.width}px/ 2)`)};
	border: none;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);

	border-radius: 4px;
	font-weight: 600;
	&:hover{
		background-color:#fffafac7;
	}
`

const Mask = styled.div`
	width: 450px;
	height: 450px;
	position: absolute;
	background-color: #615f5fc7;
	margin-left: -6px;
	display: flex;
	border-radius: 4px;
`

const Banner = styled.div`
	margin: 160px auto 0 auto;
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

const MoveContainerContainer = styled.div`
	background-color: burlywood;
	margin-left: 50px;
	padding: 5px;
	border-radius: 4px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
`

const MoveHistoryContainer = styled.div`
	background-color: sienna;
	border-radius: 4px;
	/* margin-left: 50px; */
	padding: 10px 0;
	height: 420px;
	/* width: 110px; */
	/* overflow: auto; */
	/* box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75); */
`

const MoveContainer = styled.div`
	height: 410px;
	width: 90px;
	overflow: auto;
	&::-webkit-scrollbar {
		display: none;
	}
  	-ms-overflow-style: none;  /* IE and Edge */
 	scrollbar-width: none;  /* Firefox */
	padding-left: 15px;
	/* margin-top: 5px; */
`

const TitleContainer = styled.div`
	height:20px;
	position: relative;
	margin-top: -5px;
	&:after{
		content: "";
		position: absolute;
		top: 0px;
		bottom: 0;
		box-shadow: 0px 5px 8px -2px rgba(0,0,0,0.60);
		width: 105px;
		height: 20px;
		display: inherit;
	}	
`

const Title = styled.span`
	color:#f1d2ab;
	font-weight: 600;
	text-align: center;
	display: grid;
	padding-bottom: 5px;
`

const MoveLi = styled.li`
	color:#f1d2ab;
	margin: 2.5px 0;
	&::marker {
		color: ${(props) => (props.color === "w" && 'white')
		|| 'black'
	};
	}
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

const pieceSwitch = (piece, size) => {
	switch (piece) {
		case 'k':
			return (<img width={size} height={size} src={b_k}></img>)
			break;
		case 'K':
			return (<img width={size} height={size} src={w_k}></img>)
			break;
		case 'q':
			return (<img width={size} height={size} src={b_q}></img>)
			break;
		case 'Q':
			return (<img width={size} height={size} src={w_q}></img>)
			break;
		case 'n':
			return (<img width={size} height={size} src={b_n}></img>)
			break;
		case 'N':
			return (<img width={size} height={size} src={w_n}></img>)
			break;
		case 'r':
			return (<img width={size} height={size} src={b_r}></img>)
			break;
		case 'R':
			return (<img width={size} height={size} src={w_r}></img>)
			break;
		case 'b':
			return (<img width={size} height={size} src={b_b}></img>)
			break;
		case 'B':
			return (<img width={size} height={size} src={w_b}></img>)
			break;
		case 'p':
			return (<img width={size} height={size} src={b_p}></img>)
			break;
		case 'P':
			return (<img width={size} height={size} src={w_p}></img>)
			break;
		case '':
			return (null)
			break;

		default:
			return (null)
			break;
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

const startStream = () => {
	console.log('Connecting to event stream')
	const eventSourceInitDict = {
		headers: {
			Authorization: `Bearer `
		}
	}
	const eventSource = new EventSourcePoly(`/fen`, eventSourceInitDict)
	eventSource.onopen = (m) => {
		console.log('Connected!', m)
	}
	eventSource.onerror = (e) => console.log(e)
	eventSource.onmessage = (e) => {
		const data = JSON.parse(e.data)
		console.log('stream data', data)
	}

	return () => {
		eventSource.close()
		console.log('eventSource closed!')
	}
}

const Board = () => {
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
	const [selectedPiece, setSelectedPiece] = useState({})
	const [allowedMoves, setAllowedMoves] = useState([])
	const [botThinking, setBotThinking] = useState(false)

	useEffect(() => {
		if (document.getElementById('start')) {
			setStartWidth(document.getElementById('start').scrollWidth)
		}
	}, [winner]);

	const squareInLastMove = (i, j, r1c, r1r, r2c, r2r) => {
		return (j === r1c && i === r1r) || (j === r2c && i === r2r)
	}

	const setCastling = ({castle = false, king = false, qtower = false, ktower = false}) => {
		if (castle || king) {
			let extras = fenExtras.castling.replace("K", "")
			extras = extras.replace("Q", "")
			setFenExtras({ toMove: 'b', castling: extras })
		}else if(qtower){
			let extras = fenExtras.castling.replace("Q", "")
			setFenExtras({ toMove: 'b', castling: extras })
		}else if(ktower){
			let extras = fenExtras.castling.replace("K", "")
			setFenExtras({ toMove: 'b', castling: extras })
		}else{
			setFenExtras({ toMove: 'b', castling: fenExtras.castling })
		}
	}

	const clickSquare = (coords) => {
		const fenBoard = new FENBoard(fen)
		const pieceSide = fenBoard.board[coords.y][coords.x] === fenBoard.board[coords.y][coords.x].toUpperCase() ? 0 : 1

		if (fenBoard.board[coords.y][coords.x] !== "" && pieceSide === playAs && playAs === turn) {
			let newAllowedMoves = []
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
						newAllowedMoves.push(`${r2c}-${r2r}`)
					}
				}

			}
			setAllowedMoves(newAllowedMoves)
		}
		let moved = false
		let move = ''
		if (allowedMoves.includes(`${coords.x}-${coords.y}`)) {
			fenBoard.move(`${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}`, `${String.fromCharCode(coords.x + 97)}${8 - coords.y}`)
			setFen(fenBoard.fen)
			moved = true
			move = `${(fenBoard.board[coords.y][coords.x] !== 'P' ? fenBoard.board[coords.y][coords.x] : '')}${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}-${String.fromCharCode(coords.x + 97)}${8 - coords.y}`

		} else if (allowedMoves.includes(`O-O`)) {
			if (fenExtras.toMove === 'w' && coords.x === 6 && coords.y === 7) {
				fenBoard.move(`e1`, `g1`);
				fenBoard.move(`h1`, `f1`);
			} else if (fenExtras.toMove === 'b' && coords.x === 6 && coords.y === 0) {
				fenBoard.move(`e7`, `g7`);
				fenBoard.move(`h7`, `f7`);
			} else {
				return;
			}
			moved = true
			move = 'O-O'
		} else if (allowedMoves.includes(`O-O-O`)) {
			if (fenExtras.toMove === 'w' && coords.x === 2 && coords.y === 7) {
				fenBoard.move(`e1`, `c1`);
				fenBoard.move(`a1`, `d1`);
			} else if (fenExtras.toMove === 'b' && coords.x === 2 && coords.y === 0) {
				fenBoard.move(`e7`, `c7`);
				fenBoard.move(`a7`, `d7`);
			} else {
				return;
			}
			moved = true
			move = 'O-O-O'
		}
		setLastMove(move)

		if (moved) {
			setTurn(1)
			setFen(fenBoard.fen)
			setSelectedPiece({})
			setAllowedMoves([])
			
			if (fenExtras.toMove === 'w') {
				if (move === 'O-O' || move === 'O-O-O') {
					setCastling({castle: true})
				}else if(move[0] === 'K'){
					setCastling({king: true})
				}else if(move[0] === 'R' && move[1] === 'h' && move[2] === '1'){
					setCastling({ktower: true})
				}else if(move[0] === 'R' && move[1] === 'a' && move[2] === '1'){
					setCastling({qtower: true})
				} else {
					setCastling({})
				}
			} else {
				setFenExtras({ toMove: 'w', castling: fenExtras.castling })
			}
			moveHistory.push(
				{
					move,
					color: fenExtras.toMove
				})
		}

		return (pieceSide === playAs && playAs === turn)
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
				const allowed = !movedPiece && (allowedMoves.includes(`${j}-${i}`) || shortCastle || longCastle)
				
				tempItems.push(
					<Square id={`${j}${i}`} key={`${j}-${i}`} white={(i * 7 + j) % 2 === 0} allowed={allowed} coords={{ x: j, y: i }} clickCallback={clickSquare} moved={moved}>
						{pieceSwitch(fenBoard.board[i][j], 46)}
					</Square>
				)
			}
		}
		setItems(tempItems)
	}

	const startGame = () => {
		setWinner(null)
		setRunning(true)
		setFenHistory({})
		setFen(`${new FENBoard("start").fen}`)
		setFenExtras({ toMove: 'w', castling: 'KQkq' })
		setMoveHistory([])
		setLastMove("")
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
	}

	const updateScroll = () => {
		var element = document.getElementById("moveContainer");
		element.scrollTop = element.scrollHeight;
	}

	const playBotTurn = (fen) => {
		console.log('play fen', fen)
		setBotThinking(true)
		if (fen.length > 0) {
			playTurn({ fen })
				.then((result) => {
					console.log('result', result)
					console.log('receive fen', result.data.fen)
					if (result.data.fen.includes("wins")) {
						setRunning(false)
						setWinnerText(result.data.fen)
						if (result.data.fen.includes("White")) {
							setWinner("w")
						} else {
							setWinner("b")
						}
						setBotThinking(false)

					} else if (result.data.fen.includes("Draw")) {
						setRunning(false)
						setWinnerText(result.data.fen)
						setWinner("d")
						setBotThinking(false)
					} else {
						if (result.data.fen in fenHistory) {
							fenHistory[result.data.fen]++
							if (fenHistory[result.data.fen] === 3) {
								setRunning(false)
								setWinnerText("Draw")
								setWinner("d")
							}
						} else {
							fenHistory[result.data.fen] = 1
						}
						moveHistory.push(
							{
								move: result.data.lastMove,
								color: result.data.fen.includes(" b") ? "w" : "b"
							})
						let moves = []
						for (const move in result.data) {
							if (!isNaN(move)) {
								moves.push(result.data[move])
							}
						}
						setLegalMoves(moves)
						setTurn(result.data.fen.includes(" b") ? 1 : 0)
						setLastMove(result.data.lastMove)
						setTimeout(() => {
							updateScroll()
						}, 100)
						// fenBoard.fen = result.data.fen
						var splitfen = result.data.fen.split(' ')
						setFen(splitfen[0])
						setFenExtras({ toMove: splitfen[1], castling: splitfen[2] || '' })
						setBotThinking(false)
					}
				})
		}
	}

	useEffect(() => {
		setBoard(false)

		if (fenExtras.toMove === 'b' && !botThinking) {
			playBotTurn(`${fen} ${fenExtras.toMove} ${fenExtras.castling}`)
		}
	}, [fen, fenExtras, allowedMoves, selectedPiece, legalMoves, botThinking, turn])

	return (
		<>
			<BoardContainer>
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
				<CoordinatesLetters offsetBottom={-20} />
				<Container>
					<CoordinatesNumbers offsetRight={-15} />
					<BoardBorder>
						<BoardStyle>
							{items}
						</BoardStyle>
					</BoardBorder>
					<CoordinatesNumbers offsetLeft={-15} />
					<MoveContainerContainer>
						<MoveHistoryContainer>
							<TitleContainer>
								<Title >Move history</Title>
							</TitleContainer>
							<MoveContainer id="moveContainer">
								{moveHistory.map((move) => (
									<MoveLi key={move.move} color={move.color}>{move.move}</MoveLi>
								))}
							</MoveContainer>
						</MoveHistoryContainer>
					</MoveContainerContainer>
					{!running && (
						<Mask>
							{winner && (
								<Banner
									winner={winner}>
									<BannerText>{winnerText}</BannerText>
								</Banner>
							)}

							<StartButton
								id="start"
								width={startWidth}
								onClick={startGame}>{winner ? "Start new game" : "Start game"}
							</StartButton>
						</Mask>
					)}
				</Container>
				<CoordinatesLetters offsetTop={-20} />
			</BoardContainer >
		</>
	)
}

export default Board
