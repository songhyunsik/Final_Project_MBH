import sys
from google.cloud import pubsub_v1

def publish_message(project_id, topic_id, message):
    publisher = pubsub_v1.PublisherClient()
    topic_path = publisher.topic_path(project_id, topic_id)
    data = message.encode("utf-8")
    future = publisher.publish(topic_path, data)
    print(f"Published message ID: {future.result()}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python send_to_pubsub.py <project_id> <topic_id> <message>")
        sys.exit(1)

    project_id = sys.argv[1]
    topic_id = sys.argv[2]
    message = sys.argv[3]

    publish_message(project_id, topic_id, message)