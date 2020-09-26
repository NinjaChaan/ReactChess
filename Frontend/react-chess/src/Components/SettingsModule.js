import React, { useState, useEffect } from 'react'
import styled, { css } from 'styled-components'
import { device } from '../devices'

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
`

const SettingsButton = styled.button`
	background-color: sienna;
	min-width: 75px;
	width: fit-content;
	height: 35px;
	@media ${device.tablet}{
		width: max-content;
		height: 30px;
	}
	border-radius: 4px;
	color: #f1d2ab;
	border: 2px solid burlywood;
	margin-right: 10px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	outline: none;
	&:hover {
		background-color: #a5603f;
		opacity: 100%;
	};
	&:last-of-type{
		margin-right: 0px;
	}
	&:active {
		transform: translateY(2px);
	}
`

const DifficultyButton = styled(SettingsButton)`
	background-color: sienna;
	min-width: 75px;
	
	border-radius: 4px;
	color: #f1d2ab;
	border: 2px solid burlywood;
	margin-right: 10px;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	opacity:${(props) => (props.selected && '100%')
		|| '60%'};
	&:hover {
		background-color: #a5603f;
		opacity: 100%;
	};
	&:last-of-type{
		margin-right: 0px;
	}
`

const SettingsContainer = styled.div`
	/* position: absolute; */
	left: 50px;
	top: 10px;
	margin: 25px 150px 0 50px;
	margin: ${(props) => (props.mobile && '25px 0 0 0')};
	background-color: burlywood;
	padding: 5px;
	border-radius: 4px;
	height: max-content;
	box-shadow: 0px 0px 5px 0px rgba(0,0,0,0.75);
	/* flex-basis: 0;
	flex-grow: 1; */
	
	flex: 0 0 10%;
	max-width: max-content;
	user-select: none;
	min-width: 365px;
	min-width: ${(props) => (props.mobile && '445px')};

	@media ${device.tablet}{
		/* flex: 0 0 20%; */
		flex: 0 0 17.7%;
		max-width: max-content;
	}
`

const SettingsInner = styled.div`
	background-color: sienna;
	border-radius: 4px;
	padding: 10px;
	display: flex;
	flex-direction: column;
	width: auto;
	/* height: 100%; */
`

const SettingsGroup = styled.div`
	padding: 10px;
	display: inline-block;
	width: auto;
	/* height: 100%; */
`

const ButtonGroup = styled.div`
	display: flex;
	justify-content: center;
`

const SettingsModule = ({ difficulty, setDifficulty, getHint, showOptions, setShowOptions, mobile }) => {
	return (
		<SettingsContainer mobile={mobile}>
			<SettingsInner>
				<TitleContainer style={{ margin: "0 -10px" }} onClick={() => { setShowOptions(!showOptions) }}>
					<Title style={{ width: "1%", marginLeft: "10px" }}>{showOptions ? '▲' : '▼'}</Title>
					<Title style={{ width: "90%" }}>Settings</Title>
				</TitleContainer>
				{showOptions && (
					<SettingsInner>
						<SettingsGroup>
							<Title>Difficulty</Title>
							<ButtonGroup>
								<DifficultyButton selected={difficulty === 1} onClick={() => { setDifficulty(1) }}>Easy</DifficultyButton>
								<DifficultyButton selected={difficulty === 2} onClick={() => { setDifficulty(2) }}>Medium</DifficultyButton>
								<DifficultyButton selected={difficulty === 3} onClick={() => { setDifficulty(3) }}>Hard</DifficultyButton>
							</ButtonGroup>
						</SettingsGroup>
						<SettingsGroup>
							<Title>Get a hint</Title>
							<ButtonGroup>
								<SettingsButton onClick={() => { getHint(true) }}>Best piece</SettingsButton>
								<SettingsButton onClick={() => { getHint(false) }}>Best move</SettingsButton>
							</ButtonGroup>
						</SettingsGroup>
					</SettingsInner>
				)}
			</SettingsInner>
		</SettingsContainer>
	)
}

export default SettingsModule
