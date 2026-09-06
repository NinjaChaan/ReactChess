import React from 'react'
import styled, {keyframes} from 'styled-components'

const ellipsis = keyframes`
	0% {
		content: ""
	}
	16% {
		content: "."
	}
	32% {
		content: ".."
	}
	48% {
		content: "..."
	}
	64% {
		content: "\\00a0.."
	}
	80% {
		content: "\\00a0\\00a0."
	}
`

const Ellipsis = styled.span`
	&:after {
		content: "";
		text-align: start;
		overflow: hidden;
		display: inline-block;
		vertical-align: bottom;
		animation: ${ellipsis} 2500ms infinite;
		width: 20px;
		height: 22px;
	}
`
export default Ellipsis
