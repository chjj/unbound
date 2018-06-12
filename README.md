# unbound

Bindings to libunbound for node.js.

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

See: https://github.com/NLnetLabs/unbound/blob/master/libunbound/unbound.h

- `Unbound.version()` - Returns version string.
- `Unbound#setOption(opt, val)`
- `Unbound#getOption(opt)`
- `Unbound#setConfig(file)`
- `Unbound#setForward(addrname)`
- `Unbound#setStub(zone, addr, [isPrime=false])`
- `Unbound#setResolvConf(filename)`
- `Unbound#setHosts(filename)`
- `Unbound#addTrustAnchor(ta)`
- `Unbound#addTrustAnchorFile(file, [autr=false])`
- `Unbound#addTrustedKeys(file)`
- `Unbound#addZone(zoneName, zoneType)`
- `Unbound#removeZone(zoneName)`
- `Unbound#addData(data)`
- `Unbound#removeData(data)`
- `Unbound#resolve(name, [type=A], [class=IN])`

## Contribution and License Agreement

If you contribute code to this project, you are implicitly allowing your code
to be distributed under the MIT license. You are also implicitly verifying that
all code is your original work. `</legalese>`

## License

- Copyright (c) 2018, Christopher Jeffrey (MIT License).

See LICENSE for more info.
