import React from 'react'
import styled from 'styled-components'

const Foot = styled.footer`
	align-content: center;
`

const Footer = () => {
	return (
		<Foot>Chess pieces by <a href="https://en.wikipedia.org/wiki/User:Cburnett" title="en:User:Cburnett">Cburnett</a> - <a href="http://creativecommons.org/licenses/by-sa/3.0/" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a></Foot>
	)
}

export default Footer
