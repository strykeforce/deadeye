const CracoLessPlugin = require("craco-less");

module.exports = {
  plugins: [
    {
      plugin: CracoLessPlugin,
      options: {
        lessLoaderOptions: {
          lessOptions: {
            // modifyVars: { "@primary-color": "rgb(241, 186, 27)" },
            javascriptEnabled: true,
          },
        },
      },
    },
  ],
};
