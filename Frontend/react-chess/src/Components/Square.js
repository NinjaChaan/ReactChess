import React, { useState, useEffect } from 'react'
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
	/* border: ${(props) => (props.clicked && '3px solid yellow')
		|| '3px solid transparent'
	}; */
	border: none;
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
	}

	/* &:hover{
		box-shadow: inset 2000px 0 0 0 rgba(0, 255, 0, 0.5);
	} */
`

const Square = ({ white, moved, children, selected, id, coords, clickCallback }) => {
	const [clicked, setClicked] = useState(false)

	const selectPiece = () => {
		if (children && canClick) {
			setClicked(true)
		}
	}

	useEffect(() => {
		setClicked(selected)
	}, [selected]);

	const click = () => {
		setClicked(clickCallback(coords))
	}	

	return (
		<SquareStyle
			id={`square-${id}`}
			white={white}
			moved={moved}
			selected={clicked}
			onClick={click}
			onBlur={() => { setClicked(false) }}>
			{children}
		</SquareStyle>
	)
}

export default Square
