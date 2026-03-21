from curl_cffi import requests
import json
def headerss(cookie):
   return {
        'accept': '*/*',
        'accept-language': 'vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5',
        'content-type': 'application/x-www-form-urlencoded',
        'origin': 'https://www.facebook.com',
        'priority': 'u=1, i',
        'referer': 'https://www.facebook.com/',
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
        'x-fb-lsd': 'm9SH-Ky91VkAZqozkIAjYY',
        'cookie': cookie,
    }
def like (cookie,av_ID,dtsg,feedback_id,reaction,actor_ID,docID):
    headers = headerss(cookie)
    data = {
        'av': av_ID,
        'fb_dtsg': dtsg,
        'fb_api_caller_class': 'RelayModern',
        'fb_api_req_friendly_name': 'CometUFIFeedbackReactMutation',
        'server_timestamps': 'true',
        'variables': f'{{"input":{{"attribution_id_v2":"CometPhotoRoot.react,comet.mediaviewer.photo,via_cold_start,1773763121982,241263,,,","feedback_id":"{feedback_id}","feedback_reaction_id":"{reaction}","feedback_source":"MEDIA_VIEWER","is_tracking_encrypted":true,"tracking":[],"session_id":"0be13f78-ae34-43b5-8c07-dc3796f24d86","actor_id":"{actor_ID}","client_mutation_id":"1"}},"useDefaultActor":false,"__relay_internal__pv__CometUFIReactionsEnableShortNamerelayprovider":false}}',
        'doc_id': docID,
    }
    response = requests.post('https://www.facebook.com/api/graphql/',headers=headers, data=data)
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
            return "fail"
    except:
        return "parse_error"
def follow(cookie,av_ID,dtsg,subcribe_ID,actor_ID,docID):
    headers = headerss(cookie)
    data = {
        'av': av_ID,
        'fb_dtsg':dtsg,
        'fb_api_caller_class': 'RelayModern',
        'fb_api_req_friendly_name': 'CometUserFollowMutation',
        'server_timestamps': 'true',
        'variables': f'{{"input":{{"attribution_id_v2":"ProfileCometTimelineListViewRoot.react,comet.profile.timeline.list,via_cold_start,1773768802665,733426,250100865708545,,","is_tracking_encrypted":false,"subscribe_location":"PROFILE","subscribee_id":"{subcribe_ID}","tracking":null,"actor_id":"{actor_ID}","client_mutation_id":"1"}},"scale":1}}',
        'doc_id': docID,
    }
    response = requests.post('https://www.facebook.com/api/graphql/',headers=headers,data=data)
    text = response.text
    if text.startswith("for (;;);"):
        text = text[9:]
    try:
        j = json.loads(text)

        if "errors" in j:
            return "error"
        if "errors" in j or "data" not in j:
            return "error"
        status = j["data"]["actor_subscribe"]["subscribee"]["subscribe_status"]

        if status == "IS_SUBSCRIBED":
            return "success"
        else:
            return "fail"

    except:
        return "parse_error"
