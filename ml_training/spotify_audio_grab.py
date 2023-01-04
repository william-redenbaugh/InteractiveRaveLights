import json

creds_file = str(open("creds.json", "r").read())
creds_json = json.loads(creds_file)
spotify_client_id = creds_json["client_id"]
spotify_secret = creds_json["client_secret"]

