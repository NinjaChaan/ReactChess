import React, { useState, useEffect, useCallback } from 'react'
import styled from 'styled-components'
import EventSourcePoly from 'eventsource'
import Square from '../Components/Square'
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
// const StartButton = styled.button`
// 	height: 40px;
// 	margin: -100px auto 0 auto;
// 	border-radius: 4px;
// 	font-weight: 600;
// 	&:hover{
// 		background-color:#fffafac7;
// 	}
// `


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
	const [fen, setFen] = useState("")
	const [fenBoard, setFenboard] = useState(new FENBoard("start"))
	const [items, setItems] = useState([])
	const [startWidth, setStartWidth] = useState()
	const [fenHistory, setFenHistory] = useState({})
	const [moveHistory, setMoveHistory] = useState([])
	const [lastMove, setLastMove] = useState("")
	const [selectedPiece, setSelectedPiece] = useState({})

	useEffect(() => {
		setFenboard(new FENBoard("start"))
		setFen("blaa")
		setTurn(0)
	}, []);

	useEffect(() => {
		if (document.getElementById('start')) {
			setStartWidth(document.getElementById('start').scrollWidth)
		}
	}, [winner]);

	const squareInLastMove = (i, j, r1c, r1r, r2c, r2r) => {
		return (j === r1c && i === r1r) || (j === r2c && i === r2r)
	}

	const selectPiece = (coords) => {
		console.log('click callback', coords)
		const pieceSide = fenBoard.board[coords.x][coords.y] === fenBoard.board[coords.x][coords.y].toUpperCase() ? 0 : 1
		if (pieceSide === playAs && playAs === turn) {
			console.log('success')
			setSelectedPiece(coords)
		}
		return (pieceSide === playAs && playAs === turn)
	}

	const setBoard = () => {
		console.log('setting boards')
		setSelectedPiece({})
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
				const selected = selectedPiece.x === i && selectedPiece.y === j
				tempItems.push(
					<Square id={`${i}${j}`} key={`${i}-${j}`} white={(i * 7 + j) % 2 === 0} coords={{ x: i, y: j }} clickCallback={selectPiece} selected={selected} moved={moved}>
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
		setItems([])
		setFenboard(new FENBoard("start"))
		setFenHistory({})
		setFen(`${new FENBoard("start").fen} w KQkq`)
		setMoveHistory([])
		setLastMove("")
		setTurn(0)
		//setFen(`8/8/8/8/3q1k2/R6K/8/8 w`)
	}

	const updateScroll = () => {
		var element = document.getElementById("moveContainer");
		element.scrollTop = element.scrollHeight;
	}

	useEffect(() => {
		if (running) {
			if (fen.length > 0) {
				playTurn({ fen })
					.then((result) => {
						console.log('fen result', result)
						if (result.data.fen.includes("wins")) {
							setRunning(false)
							setWinnerText(result.data.fen)
							if (result.data.fen.includes("White")) {
								setWinner("w")
							} else {
								setWinner("b")
							}

						} else if (result.data.fen.includes("Draw")) {
							setRunning(false)
							setWinnerText(result.data.fen)
							setWinner("d")
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
							setTurn(result.data.fen.includes(" b") ? 1 : 0)
							setLastMove(result.data.lastMove)
							setTimeout(() => {
								updateScroll()
							}, 100)
							fenBoard.fen = result.data.fen
							setFen(`${result.data.fen}`)
						}
					})
			}
		}
		setBoard()
	}, [fen])

	return (
		<>
			<BoardContainer>
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
