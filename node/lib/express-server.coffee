RJs = require 'requirejs'
Modmap = require 'modmap'
Express = require 'express'
Assets = require 'connect-assets'
ServeStatic = require 'serve-static'
Browzerify = require 'browzerify'
_ = require 'lodash'
AssetManager = require 'asset-manager'

# Configure requirejs for node #############################


RJs.config Modmap.amd_node_cfg


# Export/compile our assets ################################

module.exports = App = new Express

# Mount all the asset paths
for path, dir of Modmap.web_assets
  App.use Assets _.merge {}, Modmap.connect_assets_cfg,
    paths: dir
    servePath: path
  App.use path, ServeStatic dir


# Load node_modules in the browser #########################

App.get '/require/:module', Browzerify.request_handler

# API ######################################################

App.get '/api/:version/asset-manager/listStatic', (req, res) ->
  AssetManager.listStatic (err, data) ->
    # TODO: We need generic error handling
    if err
      res.mime "text/plain"
      res.status 500
      res.send "Some unknown error occurred on the server"
      throw err
    else
      res.json data

App.get '/api/:version/asset-manager/listNodeModules', (req, res) ->
  AssetManager.listNodeModules (err, data) ->
    # TODO: We need generic error handling
    if err
      res.mime "text/plain"
      res.status 500
      res.send "Some unknown error occurred on the server"
      throw err
    else
      res.json data
