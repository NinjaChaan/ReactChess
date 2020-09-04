import React, { useState } from 'react'
import styled from 'styled-components'

const SquareStyle = styled.button`
	width: 50px;
	height: 50px;
	padding: 0;
	background-color: ${(props) => (props.white && 'burlywood')
		|| 'sienna'
	};
	border: ${(props) => (props.clicked && '3px solid yellow')
		|| '3px solid transparent'
	};

	/* &:hover{
		box-shadow: inset 2000px 0 0 0 rgba(0, 255, 0, 0.5);
	} */
`

const Square = ({ white, children }) => {
	const [clicked, setClicked] = useState(false)

	const selectPiece = () => {
		if(children){
			setClicked(true)
		}
	}

	return (
		<SquareStyle white={white} clicked={clicked} onClick={selectPiece} onBlur={() => { setClicked(false) }}>
			{children}
		</SquareStyle>
	)
}

export default Square
