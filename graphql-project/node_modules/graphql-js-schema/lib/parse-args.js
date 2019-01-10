'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = parseArgs;

var _minimist = require('minimist');

var _minimist2 = _interopRequireDefault(_minimist);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function shouldShowHelp(args) {
  return args.help || !(args['schema-file'] && args.outdir);
}

function parseArgs(rawArgs) {
  const args = (0, _minimist2.default)(rawArgs, {
    boolean: 'bundle-only',
    string: ['schema-file', 'outdir', 'schema-bundle-name', 'whitelist-config'],
    default: {
      'schema-bundle-name': 'Schema',
      'bundle-only': false
    }
  });

  if (shouldShowHelp(args)) {
    return { showHelp: true };
  }

  return {
    schemaFile: args['schema-file'],
    outdir: args.outdir,
    schemaBundleName: args['schema-bundle-name'],
    bundleOnly: args['bundle-only'],
    whitelistConfig: args['whitelist-config']
  };
}