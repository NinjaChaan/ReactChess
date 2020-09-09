import React from 'react'
import styled from 'styled-components'

const CoordinatesContainerVertical = styled.div`
	display: grid;
	padding: 25px 0;
	z-index: 1;
	margin-right: ${(props) => (`${props.offsetRight}px`)};
	margin-left: ${(props) => (`${props.offsetLeft}px`)};
`
const CoordinatesContainerHorizontal = styled.div`
	display: flex;
	margin-left: 19px;
	margin-top: ${(props) => (`${props.offsetTop}px`)};
	margin-bottom: ${(props) => (`${props.offsetBottom}px`)};
	z-index: 2;
	position: absolute;
`

const CoordinatesVertical = styled.span`
	font-size: 18px;
	margin: auto auto;
	height: 50px;
	line-height: 50px;
	color: #f1d2ab;
`
const CoordinatesHorizontal = styled.span`
	font-size: 16px;
	margin: auto 0;
	width: 50px;
	text-align: center;
	color: #f1d2ab;
`
const CoordinatesLetters = ({ offsetTop, offsetBottom }) => {
	return (
		<CoordinatesContainerHorizontal offsetTop={offsetTop} offsetBottom={offsetBottom}>
			<CoordinatesHorizontal>A</CoordinatesHorizontal>
			<CoordinatesHorizontal>B</CoordinatesHorizontal>
			<CoordinatesHorizontal>C</CoordinatesHorizontal>
			<CoordinatesHorizontal>D</CoordinatesHorizontal>
			<CoordinatesHorizontal>E</CoordinatesHorizontal>
			<CoordinatesHorizontal>F</CoordinatesHorizontal>
			<CoordinatesHorizontal>G</CoordinatesHorizontal>
			<CoordinatesHorizontal>H</CoordinatesHorizontal>
		</CoordinatesContainerHorizontal>
	)
}
const CoordinatesNumbers = ({ offsetRight, offsetLeft }) => {
	return (
		<CoordinatesContainerVertical offsetRight={offsetRight} offsetLeft={offsetLeft}>
			<CoordinatesVertical>8</CoordinatesVertical>
			<CoordinatesVertical>7</CoordinatesVertical>
			<CoordinatesVertical>6</CoordinatesVertical>
			<CoordinatesVertical>5</CoordinatesVertical>
			<CoordinatesVertical>4</CoordinatesVertical>
			<CoordinatesVertical>3</CoordinatesVertical>
			<CoordinatesVertical>2</CoordinatesVertical>
			<CoordinatesVertical>1</CoordinatesVertical>
		</CoordinatesContainerVertical>
	)
}

export {
	CoordinatesLetters,
	CoordinatesNumbers
}