const request = require('request');

request({
    url: 'https://api.foursquare.com/v2/venues/explore',
    method: 'GET',
    qs: {
        client_id: 'VDMSVK3YIYS1LWWBGFGGJHR5HZJV1EAXT4EQXD32TXYMU2CN',
        client_secret: '1LNJBXCPXKVG3OFQ041FAQZGAYS51FIMOVFRU2XH1EMGQDGK',
        ll: '40.7243,-74.0018',
        query: 'coffee',
        v: '20180323',
        limit: 1
    }
}, function (err, res, body) {
    if (err) {
        console.error(err);
    } else {
        console.log(body);
    }
});


import GraphQLClient from 'graphql-js-client';

// This is the generated type bundle from graphql-js-schema
import types from './types.js';

const client = new GraphQLClient(types, {
    url: 'https://graphql.myshopify.com/api/graphql',
    fetcherOptions: {
        headers: `Authorization: Basic aGV5LXRoZXJlLWZyZWluZCA=`
    }
});

const query = client.query((root) => {
    root.add('shop', (shop) => {
        shop.add('name');
        shop.addConnection('products', {
            args: {
                first: 10
            }
        }, (product) => {
            product.add('title');
        });
    });
});
