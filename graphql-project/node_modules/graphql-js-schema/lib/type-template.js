'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = typeTemplate;

var _babelTypes = require('babel-types');

var t = _interopRequireWildcard(_babelTypes);

var _babelTemplate = require('babel-template');

var _babelTemplate2 = _interopRequireDefault(_babelTemplate);

var _babelGenerator = require('babel-generator');

var _babelGenerator2 = _interopRequireDefault(_babelGenerator);

var _babylon = require('babylon');

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

function buildDeclaration(replacements) {
  const object = replacements.TYPE_HASH;

  return (0, _babelTemplate2.default)(`const TYPE_NAME_IDENTIFIER = ${ JSON.stringify(object, null, 2) };`)(replacements);
}
const buildExport = (0, _babelTemplate2.default)('export default TYPE_NAME_IDENTIFIER;', { sourceType: 'module' });

function typeTemplate(type) {
  const replacements = {
    TYPE_NAME_IDENTIFIER: t.identifier(type.name),
    TYPE_HASH: type
  };

  const declaration = buildDeclaration(replacements);
  const moduleExport = buildExport(replacements);

  switch (type.kind) {
    case 'OBJECT':
      if (type.name === 'Mutation') {
        return (0, _babylon.parse)(`
            ${ (0, _babelGenerator2.default)(declaration).code }
            ${ (0, _babelGenerator2.default)(moduleExport).code }
        `, { sourceType: 'module' });
      } else {
        return (0, _babylon.parse)(`
            ${ (0, _babelGenerator2.default)(declaration).code }
            ${ (0, _babelGenerator2.default)(moduleExport).code }
        `, { sourceType: 'module' });
      }
    case 'INTERFACE':
      return (0, _babylon.parse)(`
          ${ (0, _babelGenerator2.default)(declaration).code }
          ${ (0, _babelGenerator2.default)(moduleExport).code }
      `, { sourceType: 'module' });
    case 'INPUT_OBJECT':
      return (0, _babylon.parse)(`
          ${ (0, _babelGenerator2.default)(declaration).code }
          ${ (0, _babelGenerator2.default)(moduleExport).code }
      `, { sourceType: 'module' });
    default:
      return (0, _babylon.parse)(`
          ${ (0, _babelGenerator2.default)(declaration).code }
          ${ (0, _babelGenerator2.default)(moduleExport).code }
      `, { sourceType: 'module' });
  }
}