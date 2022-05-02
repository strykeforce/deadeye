const webpack = require('webpack')
const HtmlWebpackPlugin = require('html-webpack-plugin')
const { CleanWebpackPlugin } = require('clean-webpack-plugin')
const paths = require('./paths')

module.exports = {
  entry: [paths.src + '/main.jsx'],

  resolve: {
    extensions: ['.jsx', '.js'],
    modules: ['node_modules', process.env.NODE_PATH],
  },
  output: {
    path: paths.build,
    filename: '[name].bundle.js',
  },
  module: {
    rules: [
      // JavaScript
      {
        test: /\.jsx?$/,
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader',
          options: {
            presets: [
              ['@babel/preset-env', {}],
              ['@babel/preset-react', {}]
            ],
          }
        }
      },

      // Images
      {
        test: /\.(?:ico|gif|png|jpg|jpeg)$/i,
        type: 'asset/resource',
      },

      // Fonts and SVGs
      {
        test: /\.(woff(2)?|eot|ttf|otf|svg|)$/,
        type: 'asset/inline',
      },


    ],

  },
  plugins: [
    new HtmlWebpackPlugin({
      title: 'webpack Boilerplate',
      template: paths.src + '/template.html', // template file
      filename: 'index.html', // output file
    }),
    new CleanWebpackPlugin(),
  ],
}
// "@babel/plugin-proposal-class-properties": "^7.16.7",