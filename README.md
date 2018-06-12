# unbound

Bindings to [libunbound] for node.js.

## Usage

``` js
const Unbound = require('unbound');
const ub = new Unbound();
const result = await ub.resolve('google.com');
console.log(result);
```

Outputs:

``` js
{ msg: <Buffer 00 00 81 80 00 01 00 01 00 00 00 ... >,
  secure: false,
  bogus: false,
  reason: null }
```

## API

The API is a direct mapping to [libunbound calls][header].

- `Unbound.version()` - Return unbound version string.
- `Unbound#setOption(opt, val)` - Set resolver option.
- `Unbound#getOption(opt)` - Get resolver option.
- `Unbound#setConfig(file)` - Read unbound config file.
- `Unbound#setForward(addr)` - Set host to forward DNS queries to.
- `Unbound#setStub(zone, addr, [prime=false])` - Setup stub zone.
- `Unbound#setResolvConf(filename)` - Read from `resolv.conf`.
- `Unbound#setHosts(filename)` - Read from an `/etc/hosts` file.
- `Unbound#addTrustAnchor(ta)` - Add a trust anchor (DS/DNSKEY presentation).
- `Unbound#addTrustAnchorFile(file, [autr=false])` - Add trust anchor file.
  Set `autr` for auto-updating and reading.
- `Unbound#addTrustedKeys(file)` - Add bind-style trust anchors.
- `Unbound#addZone(zoneName, zoneType)` - Add a zone to the local authority
  info.
- `Unbound#removeZone(zoneName)` - Remove zone.
- `Unbound#addData(data)` - Add localdata to the local authority info.
- `Unbound#removeData(data)` - Remove data.
- `Unbound#resolve(name, [type=A], [class=IN])` - Asynchronous recursive
  resolution (returns a `Promise`). See above example.

## Contribution and License Agreement

If you contribute code to this project, you are implicitly allowing your code
to be distributed under the MIT license. You are also implicitly verifying that
all code is your original work. `</legalese>`

## License

- Copyright (c) 2018, Christopher Jeffrey (MIT License).

See LICENSE for more info.

[libunbound]: https://www.unbound.net/
[header]: https://github.com/NLnetLabs/unbound/blob/master/libunbound/unbound.h
