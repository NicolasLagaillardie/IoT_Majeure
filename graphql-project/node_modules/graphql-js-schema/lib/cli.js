#!/usr/bin/env node
'use strict';

var _fs = require('fs');

var _fs2 = _interopRequireDefault(_fs);

var _path = require('path');

var _path2 = _interopRequireDefault(_path);

var _mkdirp = require('mkdirp');

var _mkdirp2 = _interopRequireDefault(_mkdirp);

var _parseArgs = require('./parse-args');

var _parseArgs2 = _interopRequireDefault(_parseArgs);

var _help = require('./help');

var _help2 = _interopRequireDefault(_help);

var _writers = require('./writers');

var _index = require('./index');

var _index2 = _interopRequireDefault(_index);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function runCli() {
  const args = (0, _parseArgs2.default)(process.argv.slice(2));

  if (args.showHelp) {
    console.log(_help2.default);
    process.exit(0);
  }

  let whitelistConfig;

  if (args.whitelistConfig) {
    whitelistConfig = JSON.parse(_fs2.default.readFileSync(args.whitelistConfig));
  }

  const introspectionResponse = JSON.parse(_fs2.default.readFileSync(args.schemaFile));

  if (args.bundleOnly) {
    _mkdirp2.default.sync(args.outdir);

    return (0, _index.generateSchemaBundle)(introspectionResponse, args.schemaBundleName, whitelistConfig).then(bundle => {
      return (0, _writers.writeFile)(args.outdir, bundle.path, bundle.body);
    });
  } else {
    const files = (0, _index2.default)(introspectionResponse, args.schemaBundleName, whitelistConfig);

    _mkdirp2.default.sync(_path2.default.join(args.outdir, 'types'));

    return (0, _writers.writeFiles)(args.outdir, files);
  }
}

runCli().catch(error => {
  console.trace(error);
  process.exit(1);
});