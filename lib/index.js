
if (process.platform !== 'win32') {
      throw new Error('The current version only works on windows.')
} else {
  const addon = require('bindings')('electron_share_memery')
  module.exports = addon
}

const addon = require('bindings')('electron_share_memery')
module.exports = addon