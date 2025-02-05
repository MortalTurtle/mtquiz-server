
username = "test"
password = "qwerty"
auth_header_name = "X-User-Auth-Token"


async def setup_user(service_client):
    data = {"username": username, "password": password}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    return response.text


async def setup_user_login(service_client):
    data = {"username": username, "password": password}
    response = await service_client.post(
        '/v1/users/login',
        json=data
    )
    return response.text
