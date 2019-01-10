'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.writeFiles = writeFiles;
exports.writeFile = writeFile;

var _fs = require('fs');

var _fs2 = _interopRequireDefault(_fs);

var _path = require('path');

var _path2 = _interopRequireDefault(_path);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function logFileWrite(filePath) {
  console.log(`wroteFile: ${ filePath }`);
}

function writeFiles(outdir, files, quiet = false) {
  return Promise.all(files.map(file => {
    return writeFile(outdir, file.path, file.body, quiet);
  }));
}

function writeFile(outdir, filename, body, quiet = false) {
  return new Promise((resolve, reject) => {
    _fs2.default.writeFile(_path2.default.join(outdir, filename), body, err => {
      if (err) {
        reject(err);

        return;
      }

      if (!quiet) {
        logFileWrite(_path2.default.join(outdir, filename));
      }
      resolve();
    });
  });
}