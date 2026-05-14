window.bridge = (() => {
    const _url      = `ws://${location.host}/ws`;
    const _handlers = new Map();
    let   _socket   = null;
    let   _timer    = null;
    let   _live     = false;

    function _dispatch(event, data) {
        _handlers.get(event)?.forEach(fn => fn(data));
        _handlers.get('*')?.forEach(fn => fn(event, data));
    }

    function _connect() {
        if (_socket?.readyState === WebSocket.OPEN) return;
        _socket = new WebSocket(_url);
        _socket.onopen    = () => { clearTimeout(_timer); _live = true; };
        _socket.onclose   = () => { _live = false; _timer = setTimeout(_connect, 3000); };
        _socket.onerror   = () => { _live = false; };
        _socket.onmessage = ({ data }) => {
            try {
                const { event, message } = JSON.parse(data);
                if (event) _dispatch(event, message);
            } catch {}
        };
    }

    _connect();

    window.addEventListener('beforeunload', () => {
        clearTimeout(_timer);
        _socket?.close();
    });

    return {
        on(event, handler) {
            if (!_handlers.has(event)) _handlers.set(event, []);
            _handlers.get(event).push(handler);
            return this;
        },
        off(event, handler) {
            const list = _handlers.get(event);
            if (!list) return this;
            const i = list.indexOf(handler);
            if (i !== -1) list.splice(i, 1);
            return this;
        },
        once(event, handler) {
            const wrap = data => { handler(data); this.off(event, wrap); };
            return this.on(event, wrap);
        },
        get connected() { return _live; }
    };
})();
