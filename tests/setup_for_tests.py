
async def setup_user(service_client):
    data = {"username": "test", "password": "qwerty"}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    return response.text