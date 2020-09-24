
const size = {
	mobileS: '320px',
	mobileM: '375px',
	mobileL: '425px',
	tablet: '768px',
	laptop: '1024px',
	laptopL: '1440px',
	desktop: '2560px'
}

// eslint-disable-next-line import/prefer-default-export
export const device = {
	mobileS: `(min-width: ${size.mobileS})`,
	mobileM: `(min-width: ${size.mobileM})`,
	mobileL: `(min-width: ${size.mobileL})`,
	tablet: `(min-width: ${size.tablet})`,
	laptop: `(min-width: ${size.laptop})`,
	laptopL: `(min-width: ${size.laptopL})`,
	desktop: `(min-width: ${size.desktop})`,
	desktopL: `(min-width: ${size.desktop})`,


	mobileSMAX: `(max-width: ${size.mobileS})`,
	mobileMMAX: `(max-width: ${size.mobileM})`,
	mobileLMAX: `(max-width: ${size.mobileL})`,
	tabletMAX: `(max-width: ${size.tablet})`,
	laptopMAX: `(max-width: ${size.laptop})`,
	laptopLMAX: `(max-width: ${size.laptopL})`,
	desktopMAX: `(max-width: ${size.desktop})`,
	desktopLMAX: `(max-width: ${size.desktop})`
}
