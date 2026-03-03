from camoufox.server import launch_server

launch_server(
    headless=False,
    geoip=False,
    proxy={'server': 'http://localhost:4000', "username": "test", "password": "123456"},
    port=1234,
    ws_path='hello'
)
