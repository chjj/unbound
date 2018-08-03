/* eslint-env mocha */
/* eslint prefer-arrow-callback: "off" */

'use strict';

const assert = require('assert');
const Unbound = require('../');

const KSK_2010 = '. 172800 IN DS 19036 8 2'
  + ' 49AAC11D7B6F6446702E54A1607371607A1A41855200FD2CE1CDDE32F24E8FB5';

const KSK_2017 = '. 172800 IN DS 20326 8 2'
  + ' E06D44B80B8F1D39A95C0B0D7C65D08458E880409BBC683457104237C7F8EC8D';

describe('Unbound', function() {
  it('should resolve www.ietf.org.', async () => {
    const ub = new Unbound();

    assert(ub.hasOption('logfile'));
    assert(!ub.hasOption('foobar'));
    ub.tryOption('foobar', false);

    ub.setOption('logfile', null);
    ub.setOption('use-syslog', false);
    ub.tryOption('trust-anchor-signaling', false);
    ub.setOption('edns-buffer-size', 4096);
    ub.setOption('max-udp-size', 4096);
    ub.setOption('msg-cache-size', 4 << 20);
    ub.setOption('key-cache-size', 4 << 20);
    ub.setOption('neg-cache-size', 4 << 20);
    ub.setOption('do-ip4', true);
    ub.setOption('do-ip6', false);
    ub.setOption('do-udp', true);
    ub.setOption('do-tcp', false);
    ub.tryOption('qname-minimisation', true);
    ub.setOption('minimal-responses', false);

    assert.strictEqual(ub.getOption('logfile'), null);
    assert.strictEqual(ub.getOption('qname-minimisation'), true);
    assert.strictEqual(ub.getOption('max-udp-size'), 4096);

    ub.addTrustAnchor(KSK_2010);
    ub.addTrustAnchor(KSK_2017);

    const res = await ub.resolve('www.ietf.org');

    assert(res && typeof res === 'object');
    assert(Buffer.isBuffer(res.msg));
    assert(res.msg.length > 0);
    assert.strictEqual(res.secure, true);
    assert.strictEqual(res.bogus, false);
    assert.strictEqual(res.reason, null);
  });
});
