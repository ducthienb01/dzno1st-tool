# get_feedback.pyx
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import re
import json
import time
def create_driver(profile_directory):
    options = Options()
    options.add_argument(r"--user-data-dir=C:\AutomationChrome")
    options.add_argument(f"--profile-directory={profile_directory}")
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
    WebDriverWait(driver, 20).until(
        EC.any_of(
            EC.presence_of_element_located((By.CSS_SELECTOR, '[data-testid="UFI2Comment/actionLink"]')),
            EC.presence_of_element_located((By.TAG_NAME, "body"))
        )
    )
    time.sleep(4)
    
    doc_id = None
    max_attempts = 12
    
    for _ in range(max_attempts):
        logs = driver.get_log("performance")
        for log in logs:
            try:
                msg = json.loads(log["message"])["message"]
                if msg.get("method") == "Network.requestWillBeSent":
                    params = msg["params"]
                    request = params["request"]
                    if request["method"] == "POST" and "/api/graphql/" in request["url"]:
                        request_id = params["requestId"]
                        try:
                            post_data_cmd = driver.execute_cdp_cmd(
                                "Network.getRequestPostData",
                                {"requestId": request_id}
                            )
                            payload = post_data_cmd.get("postData", "")
                            if payload:
                                match = re.search(r'"doc_id"\s*:\s*"(\d+)"', payload)
                                if match:
                                    return match.group(1)
                                
                                # Regex dự phòng nếu format hơi khác
                                alt_match = re.search(r'doc_id["\']?[:=]\s*["\']?(\d+)', payload)
                                if alt_match:
                                    return alt_match.group(1)
                        except:
                            pass
            except json.JSONDecodeError:
                continue
        time.sleep(1)
    return None
def get_feedback(url: str,profile_directory:str) -> tuple:
    """
    Trả về (feedback_id, doc_id) cho action 'like'
    Nếu không tìm thấy thì trả về ("not_found", "not_found") hoặc ("", "")
    """
    driver = create_driver(profile_directory)
    try:
        driver.get(url)
        WebDriverWait(driver, 20).until(
        EC.any_of(  # Dùng any_of để linh hoạt
            EC.presence_of_element_located((By.CSS_SELECTOR, '[data-testid="UFI2Comment/actionLink"]')),
            EC.presence_of_element_located((By.CSS_SELECTOR, '[data-testid="ufi_like_link"]')),  # like button cũ/mới
            EC.presence_of_element_located((By.CSS_SELECTOR, 'div[role="button"][aria-label*="Like"]')),  # aria-label like
            EC.presence_of_element_located((By.TAG_NAME, "body"))  # fallback cơ bản nếu không có UFI
        )
        )
        html = driver.page_source
        feedback_id = None
        match = re.search(r'"feedback":{"associated_group":null,"id":"([^"]+)"', html)
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

def get_target(url: str,profile_directory:str) -> tuple:
    """
    Trả về (target_id, doc_id) cho action 'follow'
    """
    driver = create_driver(profile_directory)
    try:
        driver.get(url)
        WebDriverWait(driver, 20).until(
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