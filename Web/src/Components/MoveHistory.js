import React from 'react'
import styled from 'styled-components'
import { device } from '../devices'

const MoveContainerContainer = styled.div`
	background-color: burlywood;
	height: max-content;	
	padding: 5px;
	border-radius: 4px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	@media ${device.tabletMAX}{
		width: 445px;
		/* height: 20px; */
	}
	@media ${device.tablet}{
		margin-left: 50px;
	}
`

const MoveHistoryContainer = styled.div`
	background-color: sienna;
	border-radius: 4px;
	padding: 10px 0;
	
	height: max-content;
`

const MoveContainer = styled.div`
	height: 200px;
	width: 100%;
	@media ${device.tablet}{
		width: 175px;
		height: 405px;
	}
	overflow: auto;
	&::-webkit-scrollbar {
		display: none;
	}
  	-ms-overflow-style: none;  /* IE and Edge */
 	scrollbar-width: none;  /* Firefox */
	display: inline-flex;
	flex-wrap: wrap;
	flex-direction: row;
	align-content: flex-start;
	overflow-y: scroll;
	padding-top: 5px;
`

const MoveText = styled.span`
	color:#f1d2ab;
	max-width: 15%;
	@media ${device.tablet}{
		max-width: 35%;
	}
	flex: 0 0 35%;
	max-height: 20px;
	margin-left: 10px;
`

const MoveNumber = styled(MoveText)`
	max-width: 5%;
	flex: 0 0 5%;
	max-height: 20px;
	margin-left: 30px;
	@media ${device.tablet}{
		margin-left: 10px;
	}
`

const TitleContainer = styled.div`
	height:20px;
	position: relative;
	margin-top: -5px;
	width: auto;
	display: flex;
	&:after{
		content: "";
		position: absolute;
		top: 0px;
		bottom: 0;
		box-shadow: 0px 5px 8px -2px rgba(0,0,0,0.60);
		width: 100%;
		height: 20px;
		display: inherit;
	}	
`

const Title = styled.span`
	color:#f1d2ab;
	font-weight: 600;
	text-align: center;
	display: grid;
	padding-bottom: 5px;
	width: 100%;
`

const Movehistory = ({ moveHistory, mobile }) => {
	return (
		<MoveContainerContainer>
			<MoveHistoryContainer>
				<TitleContainer>
					<Title >Move history</Title>
				</TitleContainer>

				<MoveContainer id="moveContainer">
					{moveHistory.map((move, i) => (
						<div key={i} style={{ display: 'contents' }}>
							{(moveHistory.indexOf(move) + 1) % 2 !== 0 && (
								<MoveNumber>{(moveHistory.indexOf(move) + 2) / 2}.{(moveHistory.indexOf(move) + 2) / 2 < 10 && ' '}</MoveNumber>
							)}
							<MoveText>{move.move}</MoveText>
						</div>
					))}
				</MoveContainer>
			</MoveHistoryContainer>
		</MoveContainerContainer>
	)
}

export default Movehistory
