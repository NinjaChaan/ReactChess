import React, { useState } from 'react'
import styled, { css } from 'styled-components'

const SquareStyle = styled.button`
	position: relative;
	width: 50px;
	height: 50px;
	padding: 0;
	background-color: ${(props) => (props.white && 'burlywood')
		|| 'sienna'
	};
	box-shadow: ${(props) => (props.moved && 'inset 2000px 0 0 0 rgba(0, 255, 0, 0.25)')};
	box-shadow: ${(props) => (props.hint && 'inset 2000px 0 0 0 rgba(255, 0, 0, 0.35)')};
	border: none;
	outline: none;
	transform: ${(props) => (props.playAs === 1 && `rotate(180deg)`)};
	&:after {
		${(props) => props.selected && css`
			content: "";
			position: absolute;
			left: 2px;
			top: 2px;
			width: 46px;
			height: 46px;
			display: inherit;
			box-shadow: 0 0 0 2px yellow;
		`}	
		${(props) => props.allowed && css`
			content: "";
			position: absolute;
			left: 18.75px;
			top: 18.75px;
			width: 12.5px;
			height: 12.5px;
			display: inherit;
			border-radius:50%;
			box-shadow: inset 2000px 0 0 0 rgb(251, 248, 74);
		`}
	}
`

const Square = ({ white, moved, children, selected, allowed, hint, id, coords, clickCallback, playAs }) => {
	const [clicked, setClicked] = useState(false)

	const selectPiece = () => {
		if (children && canClick) {
			setClicked(true)
		}
	}

	const click = () => {
		setClicked(clickCallback(coords) && children !== null)
	}

	return (
		<SquareStyle
			id={`square-${id}`}
			white={white}
			moved={moved}
			selected={clicked}
			allowed={allowed}
			hint={hint}
			playAs={playAs}
			onClick={click}
			onBlur={() => { setClicked(false) }}>
			{children}
		</SquareStyle>
	)
}

export default Square
