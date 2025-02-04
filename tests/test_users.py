# Start via `make test-debug` or `make test-release`
import tests.setup_for_tests as setup_for_tests

async def test_user_get(service_client):
    id = await setup_for_tests.setup_user(service_client)
    response = await service_client.get(
        'v1/users?id=' + id
    )
    response_json = response.json()
    assert response.status == 200
    assert response_json["username"] == 'test'
    assert response_json["id"] == id
    response = await service_client.get(
        'v1/users?username=test'
    )
    response_json = response.json()
    assert response.status == 200
    assert response_json["username"] == 'test'
    assert response_json["id"] == id

async def test_user_get_something_wrong_with_parameters(service_client):
    id = await setup_for_tests.setup_user(service_client)
    response = await service_client.get(
        'v1/users'
    )
    assert response.status == 400
    response = await service_client.get(
        'v1/users?id=wrongId'
    )
    assert response.status == 404
    response = await service_client.get(
        'v1/users?username=wrongUsername'
    )
    assert response.status == 404

#TODO do :)
async def test_user_get_joined_group(service_client):
    pass

async def test_user_add(service_client):
    data = {"username": "test", "password": "qwerty"}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 200

async def test_user_add_no_pswd(service_client):
    data = {"username": "test"}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 400
    
async def test_user_add_no_username(service_client):
    data = {"password": "qwerty"}
    response = await service_client.post(
        '/v1/users',
        json=data
    )
    assert response.status == 400
