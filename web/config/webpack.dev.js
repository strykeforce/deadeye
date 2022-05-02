const { merge } = require('webpack-merge')

const common = require('./webpack.common')

module.exports = merge(common, {
  mode: 'development',

  // Control how source maps are generated
  devtool: 'inline-source-map',

  devServer: {
    historyApiFallback: true,
    open: true,
    compress: true,
    hot: true,
    port: 8080,
  },

  module: {
    rules: [
      // CSS, PostCSS, and Sass
      {
        test: /\.(less|css)$/,
        use: [
          'style-loader',
          {
            loader: 'css-loader',
            options: { sourceMap: true, importLoaders: 1, modules: false },
          },
          { loader: 'postcss-loader', options: { sourceMap: true } },
          {
            loader: 'less-loader',
            options: {
              sourceMap: true,
              lessOptions: {
                javascriptEnabled: true,
              }
            }
          }
        ],
      },]
  }
})