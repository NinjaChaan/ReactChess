import React, { useState, useEffect } from 'react'
import styled, { css } from 'styled-components'

const SquareStyle = styled.button`
	width: 50px;
	height: 50px;
	padding: 0;
	background-color: ${(props) => (props.white && 'burlywood')
		|| 'sienna'
	};
	box-shadow: ${(props) => (props.blue && 'inset 2000px 0 0 0 rgba(0, 255, 0, 0.25)')};
	/* border: ${(props) => (props.clicked && '3px solid yellow')
		|| '3px solid transparent'
	}; */
	border: none;
	&:after {
		${(props) => props.clicked && css`
			content: "";
			position: absolute;
			left: ${(props) => (props.left && `${props.left}px`)};
			width: 46px;
			height: 46px;
			display: inherit;
			top: ${(props) => (props.top && `${props.top}px`)};
			box-shadow: 0 0 0 2px yellow;
		`}	
	}

	/* &:hover{
		box-shadow: inset 2000px 0 0 0 rgba(0, 255, 0, 0.5);
	} */
`

const Square = ({ white, blue, children, id }) => {
	const [clicked, setClicked] = useState(false)
	const [top, setTop] = useState()
	const [left, setLeft] = useState()

	const selectPiece = () => {
		if (children) {
			const element = document.getElementById(`square-${id}`)
			if (element) {
				const bound = element.getBoundingClientRect()
				setTop(bound.top - document.body.getBoundingClientRect().top + 10)
				setLeft(bound.left + 2)
			}
			setClicked(true)
		}
	}

	useEffect(() => {

	}, [clicked]);

	return (
		<SquareStyle
			id={`square-${id}`}
			white={white}
			blue={blue}
			clicked={clicked}
			onClick={selectPiece}
			top={top}
			left={left}
			onBlur={() => { setClicked(false) }}>
			{children}
		</SquareStyle>
	)
}

export default Square
