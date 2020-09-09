const path = require('path');
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
		})
	],
	devServer: {
		contentBase: path.join(__dirname, 'dist'),
		compress: true,
		port: 3000
	}
};
