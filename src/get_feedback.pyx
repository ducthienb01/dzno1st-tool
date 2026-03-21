# get_feedback.pyx
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import re
import json

def create_driver():
    options = Options()
    options.add_argument("--headless=new")
    options.add_argument("--disable-gpu")
    options.add_argument("--no-sandbox")
    options.add_argument("--disable-dev-shm-usage")
    options.add_argument("--disable-extensions")
    options.add_argument("--disable-notifications")
    options.add_argument("--blink-settings=imagesEnabled=false")
    options.add_experimental_option("prefs", {
        "profile.managed_default_content_settings.images": 2,
        "profile.managed_default_content_settings.stylesheets": 2,
        "profile.managed_default_content_settings.fonts": 2
    })
    options.page_load_strategy = "eager"
    options.set_capability("goog:loggingPrefs", {"performance": "ALL"})
    return webdriver.Chrome(options=options)

def get_doc_id(driver):
    logs = driver.get_log("performance")
    for log in logs:
        message = json.loads(log["message"])["message"]
        if message["method"] == "Network.requestWillBeSent":
            request = message["params"]["request"]
            if "graphql" in request["url"]:
                postData = request.get("postData", "")
                doc_match = re.search(r'"doc_id":"(\d+)"', postData)
                if doc_match:
                    return doc_match.group(1)
    return None

def get_feedback(url: str) -> tuple:
    """
    Trả về (feedback_id, doc_id) cho action 'like'
    Nếu không tìm thấy thì trả về ("not_found", "not_found") hoặc ("", "")
    """
    driver = create_driver()
    try:
        driver.get(url)
        WebDriverWait(driver, 6).until(
            EC.presence_of_element_located((By.TAG_NAME, "body"))
        )
        html = driver.page_source
        
        feedback_id = None
        match = re.search(r'"total_comment_count":\d+.*?"id":"([^"]+)"', html)
        if match:
            feedback_id = match.group(1)
        elif (match := re.search(r'associated_group":null,"recruiting_group_unencoded_id":null,"id":"([^"]+)"', html)):
            feedback_id = match.group(1)
        elif (match := re.search(r'"CometUFISeenByCountText.react"},"id":"([^"]+)"', html)):
            feedback_id = match.group(1)
        elif (match := re.search(r'"text"},"feedback":{"id":"([^"]+)"', html)):
            feedback_id = match.group(1)
        
        doc_id = get_doc_id(driver)
        
        fid = feedback_id if feedback_id else "not_found"
        doc = doc_id if doc_id else "not_found"
        return fid, doc
    except Exception as e:
        # Optional: log lỗi nếu cần
        return "error", str(e)
    finally:
        driver.quit()

def get_target(url: str) -> tuple:
    """
    Trả về (target_id, doc_id) cho action 'follow'
    """
    driver = create_driver()
    try:
        driver.get(url)
        WebDriverWait(driver, 6).until(
            EC.presence_of_element_located((By.TAG_NAME, "body"))
        )
        html = driver.page_source
        
        match = re.search(r'"scale":1,"selectedID":"([^"]+)"', html)
        target_id = match.group(1) if match else None
        
        doc_id = get_doc_id(driver)
        
        tid = target_id if target_id else "not_found"
        doc = doc_id if doc_id else "not_found"
        return tid, doc
    except Exception as e:
        return "error", str(e)
    finally:
        driver.quit()

# Không cần if __name__ == "__main__" nữa, vì gọi từ C++