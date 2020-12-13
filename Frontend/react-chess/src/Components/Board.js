import React, { useState, useEffect, useCallback } from 'react'
import styled from 'styled-components'
import EventSource from 'eventsource'
import Square from './Square'
import SettingsModule from './SettingsModule'
import Movehistory from './Movehistory'
import Ellipsis from './Ellipsis'
import { CoordinatesNumbers, CoordinatesLetters } from './Coordinates'
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

const Board = ({ fen, fenExtras, lastMove, hintMove, showOnlyStart, legalMoves, playAs, turn, onMakeMove, enPassant, setEnPassant, children }) => {
	const [items, setItems] = useState([])
	const [selectedPiece, setSelectedPiece] = useState({})
	const [allowedMoves, setAllowedMoves] = useState([])
	const [promotion, setPromotion] = useState(false)
	const [toPromote, setToPromote] = useState({})

	const squareInLastMove = (i, j, r1c, r1r, r2c, r2r, onlyStart = false) => {
		if (onlyStart) {
			return (j === r1c && i === r1r)
		}
		return (j === r1c && i === r1r) || (j === r2c && i === r2r)
	}

	const checkHint = (i, j) => {
		const regex = RegExp('B|R|N|K|Q')
		const cleanedMove = hintMove && (regex.test(hintMove[0]) ? hintMove.substring(1) : hintMove) || "öööööö"
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

	const promote = (selection) => {
		const fenBoard = new FENBoard(fen)
		fenBoard.move(`${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}`, `${String.fromCharCode(toPromote.x + 97)}${8 - toPromote.y}`)
		fenBoard.board[toPromote.y][toPromote.x] = selection
		const move = `${((fenBoard.board[toPromote.y][toPromote.x] !== 'P' && fenBoard.board[toPromote.y][toPromote.x] !== 'p') ? fenBoard.board[toPromote.y][toPromote.x] : '')}${String.fromCharCode(selectedPiece.x + 97)}${8 - selectedPiece.y}-${String.fromCharCode(toPromote.x + 97)}${8 - toPromote.y}`
		
		setSelectedPiece({})
		setAllowedMoves([])
		onMakeMove(fenBoard.fen, move)
		setPromotion(false)
		setToPromote({})
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
			console.log('legalMoves: ', legalMoves)
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
			setSelectedPiece({})
			setAllowedMoves([])
			onMakeMove(fenBoard.fen, move)
		}

		return (pieceSide === playAs && playAs === turn)
	}


	const setBoard = (movedPiece) => {
		const fenBoard = new FENBoard(fen)
		let showMove = true;

		const regex = RegExp('B|R|N|K|Q')

		const cleanedMove = lastMove && (regex.test(lastMove[0]) ? lastMove.substring(1) : lastMove) || "öööööö"

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

	useEffect(() => {
		setBoard(false)
	}, [allowedMoves, selectedPiece, fen, fenExtras, lastMove, hintMove, legalMoves, playAs, turn]);

	return (
		<Container>
			<CoordinatesLetters playAs={playAs} />
			<CoordinatesNumbers playAs={playAs} />
			<BoardBorder>
				<BoardStyle playAs={playAs}>
					{items}
				</BoardStyle>
			</BoardBorder>
			<CoordinatesNumbers playAs={playAs} />
			{children}
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
	)
};

export default Board