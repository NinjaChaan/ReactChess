import React from 'react'
import styled from 'styled-components'
import Square from '../Components/Square'
import FENBoard from 'fen-chess-board'
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
const CoordinatesContainerVertical = styled.div`
	display: grid;
	margin-right: 10px;
`
const CoordinatesContainerHorizontal = styled.div`
	display: flex;
	margin-left: 35px;
`

const CoordinatesVertical = styled.span`
	font-size: 40px;
	margin: 0 auto;
	height: 51.25px;
`
const CoordinatesHorizontal = styled.span`
	font-size: 40px;
	margin: auto 0;
	width: 50px;
	text-align: center;
`

const BoardStyle = styled.div`
	background-color: black;
	padding: 5px;
	width: fit-content;
	height: fit-content;
	display: grid;
	grid-template-columns: repeat(8, 1fr);
	border-radius: 4px;
`
const BoardContainer = styled.div`
	margin: 0 auto;
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

const Board = () => {
	let fenboard = new FENBoard("start")
	const items = []
	for (let i = 0; i < 8; i++) {
		for (let j = 0; j < 8; j++) {
			items.push(
				<Square key={`${i}-${j}`} white={(i * 7 + j) % 2 === 0}>
					{pieceSwitch(fenboard.board[i][j], 46)}
				</Square>
			)
		}
	}

	return (
		<BoardContainer>
			<Container>
				<CoordinatesContainerVertical>
					<CoordinatesVertical>8</CoordinatesVertical>
					<CoordinatesVertical>7</CoordinatesVertical>
					<CoordinatesVertical>6</CoordinatesVertical>
					<CoordinatesVertical>5</CoordinatesVertical>
					<CoordinatesVertical>4</CoordinatesVertical>
					<CoordinatesVertical>3</CoordinatesVertical>
					<CoordinatesVertical>2</CoordinatesVertical>
					<CoordinatesVertical>1</CoordinatesVertical>
				</CoordinatesContainerVertical>
				<BoardStyle>
					{items}
				</BoardStyle>
			</Container>
			<CoordinatesContainerHorizontal>
				<CoordinatesHorizontal>A</CoordinatesHorizontal>
				<CoordinatesHorizontal>B</CoordinatesHorizontal>
				<CoordinatesHorizontal>C</CoordinatesHorizontal>
				<CoordinatesHorizontal>D</CoordinatesHorizontal>
				<CoordinatesHorizontal>E</CoordinatesHorizontal>
				<CoordinatesHorizontal>F</CoordinatesHorizontal>
				<CoordinatesHorizontal>G</CoordinatesHorizontal>
				<CoordinatesHorizontal>H</CoordinatesHorizontal>
			</CoordinatesContainerHorizontal>
			<footer>Chess pieces by <a href="https://en.wikipedia.org/wiki/User:Cburnett" title="en:User:Cburnett">Cburnett</a> - <a href="http://creativecommons.org/licenses/by-sa/3.0/" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a></footer>
		</BoardContainer>
	)
}

export default Board
