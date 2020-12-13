import React, {useEffect} from 'react'
import styled from 'styled-components'
import BoardPage from './Components/BoardPage'
import Footer from './Components/Footer'

const BoardContainer = styled.div`
	display: flex;
`


const App = () => {
	return (
		<div id="app">
			<BoardContainer>
				<BoardPage />
			</BoardContainer>
		</div>
	)
}

export default App
