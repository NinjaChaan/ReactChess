import React from 'react'
import styled from 'styled-components'
import Board from './Components/Board'
import Footer from './Components/Footer'

const BoardContainer = styled.div`
	display: flex;
`

const App = () => {
	return (
		<div>
			<BoardContainer>
				<Board />
			</BoardContainer>
		</div>
	)
}

export default App
