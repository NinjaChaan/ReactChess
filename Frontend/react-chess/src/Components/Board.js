import React, { useState, useEffect } from 'react'
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
	margin: auto;
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
	margin-left: -5px;
	display: flex;
`

const Banner = styled.div`
	margin: auto;
	background-color: #c1fc90;
	height: 30px;
	display: flex;
	border-radius: 4px;
	padding: 5px;
	width: 100%;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
`

const BannerText = styled.span`
	margin: auto;
	font-weight: 600;
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
	const [winner, setWinner] = useState()
	const [turn, setTurn] = useState(0)
	const [fen, setFen] = useState("")
	const [fenBoard, setFenboard] = useState(new FENBoard("8/2p5/8/8/8/7k/7B/4r2K w"))
	const [items, setItems] = useState([])

	useEffect(() => {
		setFenboard(new FENBoard("8/2p5/8/8/8/7k/7B/4r2K w"))
		setFen("blaa")
		console.log('set fen')
	}, []);

	const setBoard = () => {
		const tempItems = []
		for (let i = 0; i < 8; i++) {
			for (let j = 0; j < 8; j++) {
				tempItems.push(
					<Square key={`${i}-${j}`} white={(i * 7 + j) % 2 === 0}>
						{pieceSwitch(fenBoard.board[i][j], 46)}
					</Square>
				)
			}
		}
		setItems(tempItems)
	}

	const startGame = () => {
		setRunning(true)
		setFen("8/2p5/8/8/8/7k/7B/4r2K w")
		//setFen(`${new FENBoard("start").fen} w KQkq`)
	}

	useEffect(() => {
		if (running) {
			if (fen.length > 0) {
				playTurn({ fen })
					.then((result) => {
						console.log('fen result', result)
						if (result.data.includes("wins")) {
							setRunning(false)
							setWinner(result.data)
						} else {
							//const fb = fenBoard
							//fb.fen = result.data
							//setFenboard(fb)
							fenBoard.fen = result.data
							setFen(`${result.data}`)
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
					{!running && (
						<Mask>
							{!winner ? (
								<StartButton onClick={startGame}>Start game</StartButton>
							) : (
									<Banner>
										<BannerText>{winner}</BannerText>
									</Banner>
								)}
						</Mask>

					)}
				</Container>
				<CoordinatesLetters offsetTop={-20} />

			</BoardContainer >
		</>
	)

}

export default Board
