const path = require('path');
const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

module.exports = {
	// define entry file and output
	entry: {
		main: './src/index.js',
	},
	output: {
		path: path.resolve('./dist'),
		filename: '[name].js'
	},
	// define babel loader
	module: {
		rules: [
			{
				test: /\.css$/i,
				use: ['style-loader', 'css-loader'],
			},
			{
				test: /\.(png|jpe?g|gif|svg)$/i,
				use: [
					{
						loader: 'file-loader',
					},
				],
			},
			{ test: /\.(js|jsx)$/, loader: 'babel-loader', exclude: /node_modules/ }
		]
	},
	plugins: [
		new HtmlWebpackPlugin({
			template: "./src/index.html",
			filename: "./index.html"
		}),
		new webpack.DefinePlugin({
			'process.env.API_BASE': JSON.stringify(process.env.API_BASE || ''),
			'process.env.PVP_ENABLED': JSON.stringify(process.env.PVP_ENABLED || '1')
		})
	],
	devServer: {
		contentBase: path.join(__dirname, 'dist'),
		compress: true,
		port: 3000
	}
};
