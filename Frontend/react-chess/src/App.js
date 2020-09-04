import React from 'react'
import styled from 'styled-components'
import Board from './Components/Board'

const BoardContainer = styled.div`
	display: flex;
`

const App = () => {
	return (
		<BoardContainer>
			<Board/>
		</BoardContainer>
	)
}

export default App
