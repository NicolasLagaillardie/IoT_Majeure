'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = bundleTemplate;

var _path = require('path');

var _path2 = _interopRequireDefault(_path);

var _babelTypes = require('babel-types');

var t = _interopRequireWildcard(_babelTypes);

var _babelTemplate = require('babel-template');

var _babelTemplate2 = _interopRequireDefault(_babelTemplate);

var _babelGenerator = require('babel-generator');

var _babelGenerator2 = _interopRequireDefault(_babelGenerator);

var _babylon = require('babylon');

function _interopRequireWildcard(obj) { if (obj && obj.__esModule) { return obj; } else { var newObj = {}; if (obj != null) { for (var key in obj) { if (Object.prototype.hasOwnProperty.call(obj, key)) newObj[key] = obj[key]; } } newObj.default = obj; return newObj; } }

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function buildImport(replacements) {
  const modulePath = replacements.TYPE_MODULE_PATH.value;

  return (0, _babelTemplate2.default)(`import TYPE_NAME_IDENTIFIER from "./${ modulePath }";`, { sourceType: 'module' })(replacements);
}
const buildDeclaration = (0, _babelTemplate2.default)('const BUNDLE_MODULE_NAME = {types: {}};');
const buildModuleAssignment = (0, _babelTemplate2.default)('BUNDLE_MODULE_NAME.types[TYPE_NAME] = TYPE_NAME_IDENTIFIER;');
const buildRootLevelAssignment = (0, _babelTemplate2.default)('BUNDLE_MODULE_NAME.PROPERTY_NAME = PROPERTY_VALUE;');
const buildExport = (0, _babelTemplate2.default)('export default recursivelyFreezeObject(BUNDLE_MODULE_NAME);', { sourceType: 'module' });

function bundleTemplate({ queryTypeName, mutationTypeName, subscriptionTypeName }, types, bundleModuleName) {
  const BUNDLE_MODULE_NAME = t.identifier(bundleModuleName);
  const QUERY_TYPE_NAME = queryTypeName ? t.stringLiteral(queryTypeName) : t.nullLiteral();
  const MUTATION_TYPE_NAME = mutationTypeName ? t.stringLiteral(mutationTypeName) : t.nullLiteral();
  const SUBSCRIPTION_TYPE_NAME = subscriptionTypeName ? t.stringLiteral(subscriptionTypeName) : t.nullLiteral();

  const typeConfigs = types.map(type => {
    return {
      TYPE_NAME: t.stringLiteral(type.name),
      TYPE_NAME_IDENTIFIER: t.identifier(type.name),
      TYPE_MODULE_PATH: t.stringLiteral(_path2.default.join(type.path.replace(/\.js$/, ''))),
      BUNDLE_MODULE_NAME
    };
  });

  const imports = typeConfigs.map(typeConfig => buildImport(typeConfig));
  const declaration = buildDeclaration({ BUNDLE_MODULE_NAME });
  const assignments = typeConfigs.map(typeConfig => buildModuleAssignment(typeConfig));
  const queryTypeAssignment = buildRootLevelAssignment({ BUNDLE_MODULE_NAME, PROPERTY_NAME: t.identifier('queryType'), PROPERTY_VALUE: QUERY_TYPE_NAME });
  const mutationTypeAssignment = buildRootLevelAssignment({ BUNDLE_MODULE_NAME, PROPERTY_NAME: t.identifier('mutationType'), PROPERTY_VALUE: MUTATION_TYPE_NAME });
  const subscriptionTypeAssignment = buildRootLevelAssignment({ BUNDLE_MODULE_NAME, PROPERTY_NAME: t.identifier('subscriptionType'), PROPERTY_VALUE: SUBSCRIPTION_TYPE_NAME });
  const moduleExport = buildExport({ BUNDLE_MODULE_NAME });

  return (0, _babylon.parse)(`
    ${ imports.map(ast => (0, _babelGenerator2.default)(ast).code).join('\n') }
    ${ (0, _babelGenerator2.default)(declaration).code }
    ${ assignments.map(ast => (0, _babelGenerator2.default)(ast).code).join('\n') }
    ${ (0, _babelGenerator2.default)(queryTypeAssignment).code }
    ${ (0, _babelGenerator2.default)(mutationTypeAssignment).code }
    ${ (0, _babelGenerator2.default)(subscriptionTypeAssignment).code }

    function recursivelyFreezeObject(structure) {
      Object.getOwnPropertyNames(structure).forEach((key) => {
        const value = structure[key];
        if (value && typeof value === 'object') {
          recursivelyFreezeObject(value);
        }
      });
      Object.freeze(structure);
      return structure;
    }

    ${ (0, _babelGenerator2.default)(moduleExport).code }
  `, { sourceType: 'module' });
}