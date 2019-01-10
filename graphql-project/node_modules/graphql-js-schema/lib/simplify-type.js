'use strict';

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.default = simplifyType;

var _implementsNode = require('./helpers/implements-node');

var _implementsNode2 = _interopRequireDefault(_implementsNode);

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

function getBaseTypeName(type) {
  if (type.ofType) {
    return getBaseTypeName(type.ofType);
  } else {
    return type.name;
  }
}

function getBaseTypeKind(type) {
  if (type.ofType) {
    return getBaseTypeKind(type.ofType);
  } else {
    return type.kind;
  }
}

function fieldBaseTypes(fields) {
  return fields.reduce((acc, field) => {
    acc[field.name] = getBaseTypeName(field.type);

    return acc;
  }, {});
}

function relayInputObjectBaseTypes(fields) {
  return fields.reduce((acc, field) => {
    const inputArg = field.args.find(isInputArg);

    if (inputArg) {
      acc[field.name] = getBaseTypeName(inputArg.type);
    }

    return acc;
  }, {});
}

function isInputArg(arg) {
  return arg.name === 'input' && getBaseTypeKind(arg.type) === 'INPUT_OBJECT';
}

function possibleTypes(interfaceType) {
  return interfaceType.possibleTypes.map(t => t.name);
}

function simplifyType(type, whitelist = false) {
  const simplifiedType = {
    name: type.name,
    kind: type.kind
  };

  function filterTypes(baseTypes) {
    const whitelistFields = whitelist && whitelist[simplifiedType.name];

    if (!whitelistFields) {
      return baseTypes;
    }

    return Object.keys(baseTypes).reduce((acc, field) => {
      const baseType = baseTypes[field];

      if (whitelistFields.includes(field)) {
        acc[field] = baseType;
      }

      return acc;
    }, {});
  }

  switch (type.kind) {
    case 'OBJECT':
      simplifiedType.fieldBaseTypes = filterTypes(fieldBaseTypes(type.fields));
      simplifiedType.implementsNode = (0, _implementsNode2.default)(type);

      if (type.name === 'Mutation') {
        simplifiedType.relayInputObjectBaseTypes = relayInputObjectBaseTypes(type.fields);
      }

      return simplifiedType;
    case 'INTERFACE':
      simplifiedType.fieldBaseTypes = filterTypes(fieldBaseTypes(type.fields));
      simplifiedType.possibleTypes = possibleTypes(type);

      return simplifiedType;
    case 'INPUT_OBJECT':
      simplifiedType.inputFieldBaseTypes = filterTypes(fieldBaseTypes(type.inputFields));

      return simplifiedType;
    default:
      return simplifiedType;
  }
}