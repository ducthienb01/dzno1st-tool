import requests
import json
def headerss(cookie):
   return {
        'accept': '*/*',
        'accept-language': 'vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5',
        'content-type': 'application/x-www-form-urlencoded',
        'origin': 'https://www.facebook.com',
        'priority': 'u=1, i',
        'sec-ch-prefers-color-scheme': 'light',
        'sec-ch-ua': '"Chromium";v="146", "Not-A.Brand";v="24", "Google Chrome";v="146"',
        'sec-ch-ua-full-version-list': '"Chromium";v="146.0.7680.80", "Not-A.Brand";v="24.0.0.0", "Google Chrome";v="146.0.7680.80"',
        'sec-ch-ua-mobile': '?0',
        'sec-ch-ua-model': '""',
        'sec-ch-ua-platform': '"Windows"',
        'sec-ch-ua-platform-version': '"10.0.0"',
        'sec-fetch-dest': 'empty',
        'sec-fetch-mode': 'cors',
        'sec-fetch-site': 'same-origin',
        'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36',
        'x-asbd-id': '359341',
        'x-fb-friendly-name': 'CometUFIFeedbackReactMutation',
        'cookie': cookie,
    }
def like (cookie,av_ID,dtsg,feedback_id,reaction,session_id):
    headers = {
        'accept': '*/*',
        'accept-language': 'vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5',
        'content-type': 'application/x-www-form-urlencoded',
        'origin': 'https://www.facebook.com',
        'priority': 'u=1, i',
        'sec-ch-ua': '"Chromium";v="146", "Not-A.Brand";v="24", "Google Chrome";v="146"',
        'sec-ch-ua-full-version-list': '"Chromium";v="146.0.7680.80", "Not-A.Brand";v="24.0.0.0", "Google Chrome";v="146.0.7680.80"',
        'sec-ch-ua-mobile': '?0',
        'sec-ch-ua-model': '""',
        'sec-ch-ua-platform': '"Windows"',
        'sec-ch-ua-platform-version': '"10.0.0"',
        'sec-fetch-dest': 'empty',
        'sec-fetch-mode': 'cors',
        'sec-fetch-site': 'same-origin',
        'user-agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/146.0.0.0 Safari/537.36',
        'x-asbd-id': '359341',
        'x-fb-friendly-name': 'CometUFIFeedbackReactMutation',
        'cookie':cookie,
    }
    variables = f'''{{
        "input":{{
            "attribution_id_v2":"CometSinglePostDialogRoot.react,comet.post.single_dialog,via_cold_start,1774170048745,10832,,,",
            "feedback_id":"{feedback_id}",
            "feedback_reaction_id":"{reaction}",
            "feedback_source":"OBJECT",
            "is_tracking_encrypted":true,
            "tracking":["AZaGyYLNaEJ7GfrFgDqiD4BnjpjPOTO5w1Sd9FgTXTG6GnqIGAOJLPYhkBylAoc-HLYHwUOPIpL_uWs5amt5lKXHI-4zC3a-8nCCPwTb5pwbLqejeRhRfjRxrJOVucGfPr6DYet6bSxwyD1O47FmKEzUf-sMU2E_V8F9WK4dQm82lSYnJqbGvQCf8FPKDv-aKiayug9Pej0y-2J0d5lsgqk49N5ibzBvJadxxcCJ_Wb2Au6GFrNvR-FRzfGp2MeTV1ZVK8uO1om2gX4rw9kRijuoPFpWTRB1ynx5JHRkMznquh4iPjLDVsikVrkbkMQsc6oql7v2mnTrQE0XhSzk71hhd2K7eL_nr2NwBgTebW8_HZxB1du9jtFyP7EHacdpsWH6AP_PmQ-D98GUAtnO2lcCFQALAXjEKfr17FpIQTC0xTFDwQlVg3BraDRNpEmSBUAlnx2iPGEa7fMHgiljE9yRIZHWLLH0N5dHWiu16GRnt0tWT9xCh2jyR5KHzpBlBydh18ChFk6U-xT-pu6xsfk5HMYkmguUVVAF0tXvxYcindOH24PgXRoBI93b12jffRnf4oLwAmnXFluJOJioiTIyCbjKJMCrQypNBJlVSOkGuuIH9ETgQFEZ-XXzHFBtNuMXAsVnizqmsOMmkfzrFPaO_nAun0QzVj30fMxkU6V6gOgCwb9IwstJK-XG4K_66STt_ksgUwSk4Q2VfD6KP0W_ArqV9FKQdhcMHjJoNyyYoKrDEBJJMW9bfEQXB8pvmK7BTLEDBO0CPsVgBr8wtbmHavJsP1Ncy9ZuXPixBXTGMrAVre3GdYgb0AmjMd5rFhn1oOt4wmiCGYLcxbtVt-LxVPv2H0qSxKoPV6_CYliXKVyvDK6pcscpvYNIHqZJpCJERA0fTFzeoUgzaqj0zoTgom_ggt6qFRawnzq4mSKZYfUrsZhQBbmZ4VxfuZq9ZiUNpxNfY0jRarCr9gN80pJn3aWT5U-E2uXil06dpGS4BIuTt0dPTuHr24hjuU0S5iI7RxkTGaQ2dTB-PkPduZzQyV4XjkGHGJSp7BId4DXSzMI77FIObOXAqOMeRSwk_eEAaMUBy0CAJdaqKl9UKUxWEB8dol9Ggxqhlm5RWWHPi1K5aP8h93hIpqjIz2Ks8iWgJFWS9roD34oYuSDUuZ9V4YCUDWsbUwM-5SAgE302eKwZlrEm7s0NtZDYKqMMWeMedw2yPFw4qSxbDEs2jV9tJKiqsVjNkVoQvP-8cquX4mmp6x_UFxKzq2EDc6IxwVEtsTunUaPKCvTmFLPGIxfl"],
            "session_id":"{session_id}",
            "actor_id":"{av_ID}",
            "client_mutation_id":"1"
        }},
        "useDefaultActor":false,
        "__relay_internal__pv__CometUFIReactionsEnableShortNamerelayprovider":false
    }}'''
    data = {
        'av': av_ID,
        '__user': av_ID,
        'fb_dtsg': dtsg,
        'fb_api_caller_class': 'RelayModern',
        'fb_api_req_friendly_name': 'CometUFIFeedbackReactMutation',
        'server_timestamps': 'true',
        'variables': variables,
        'doc_id': '33371893662453814',
    }
    response = requests.post('https://www.facebook.com/api/graphql/', headers=headers, data=data)
    text = response.text
    if text.startswith("for (;;);"):
        text = text[9:]
    try:
        j = json.loads(text)
        if "errors" in j:
            return "error"
        current = j["data"]["feedback_react"]["feedback"]["viewer_feedback_reaction_info"]["id"]

        if str(current) == str(reaction):
            return "success"
        else:
            return "false"
    except:
        return "parse_error"
def follow(cookie,av_ID,dtsg,subcribe_ID):
    headers = headerss(cookie)
    subcribe_id = subcribe_ID
    variables = f'''{{
    "input":{{
        "attribution_id_v2":"ProfileCometCollectionRoot.react,comet.profile.collection.reels_tab,via_cold_start,1774176657036,226052,250100865708545,,",
        "is_tracking_encrypted":false,
        "subscribe_location":"PROFILE",
        "subscribee_id":"{subcribe_id}",
        "tracking":null,
        "actor_id":"{av_ID}",
        "client_mutation_id":"1"
    }},
    "scale":1
    }}'''
    data = {
        'av': av_ID,
        '__user':av_ID,
        'fb_dtsg': dtsg,
        'fb_api_caller_class': 'RelayModern',
        'fb_api_req_friendly_name': 'CometUserFollowMutation',
        'server_timestamps': 'true',
        'variables':variables,
        'doc_id': '26720947580894156',
    }
    response = requests.post('https://www.facebook.com/api/graphql/', headers=headers, data=data)
    text =response.text
    if text.startswith("for (;;);"):
        text = text[9:]
    try:
        j = json.loads(text)
        if "data" in j and j["data"].get("actor_subscribe") is not None:
            subscribee = j["data"]["actor_subscribe"].get("subscribee", {})
            status = subscribee.get("subscribe_status")
            if status == "IS_SUBSCRIBED":
                return "success"
            else:
                print(f"Status không phải subscribed: {status}")
                return "false"
        return "false"
    except json.JSONDecodeError:
        print("JSON parse lỗi:", text)
        return "false"
    except Exception as e:
        print("Lỗi khác:", str(e))
        return "false"