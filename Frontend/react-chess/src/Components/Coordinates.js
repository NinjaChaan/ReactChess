import React from 'react'
import styled from 'styled-components'

const CoordinatesContainerVertical = styled.div`
	display: grid;
	padding: 25px 0;
	z-index: 1;
	margin-right: -15px;
	margin-left: -15px;
`
const CoordinatesContainerHorizontal = styled.div`
	display: flex;
	margin-left: 4px;
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
const CoordinatesLetters = ({ offsetTop, offsetBottom, playAs }) => {
	const arr = playAs === 0 ? ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'] : ['H', 'G', 'F', 'E', 'D', 'C', 'B', 'A']
	return (
		<CoordinatesContainerHorizontal offsetTop={offsetTop} offsetBottom={offsetBottom}>
			<CoordinatesHorizontal>{arr[0]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[1]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[2]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[3]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[4]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[5]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[6]}</CoordinatesHorizontal>
			<CoordinatesHorizontal>{arr[7]}</CoordinatesHorizontal>
		</CoordinatesContainerHorizontal>
	)
}
const CoordinatesNumbers = ({ offsetRight, offsetLeft, playAs }) => {
	const arr = playAs === 0 ? [8, 7, 6, 5, 4, 3, 2, 1] : [1, 2, 3, 4, 5, 6, 7, 8]
	return (
		<CoordinatesContainerVertical offsetRight={offsetRight} offsetLeft={offsetLeft}>
			<CoordinatesVertical>{arr[0]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[1]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[2]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[3]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[4]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[5]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[6]}</CoordinatesVertical>
			<CoordinatesVertical>{arr[7]}</CoordinatesVertical>
		</CoordinatesContainerVertical>
	)
}

export {
	CoordinatesLetters,
	CoordinatesNumbers
}