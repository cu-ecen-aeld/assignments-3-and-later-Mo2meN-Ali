#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {

class linkedList {
public:
    struct node {
        pthread_t threadID;
        int32_t commSocketPortNo;
        struct node *next;
    };

    linkedList(): head(nullptr) { }
    linkedList(pthread_t threadID, int32_t commSocketPortNo) 
    {
        struct node* item = 
            static_cast<struct node*>(malloc(sizeof(struct node)));
        if (nullptr == item) {
            throw;
        }
        item->commSocketPortNo = commSocketPortNo;
        item->threadID = threadID;
        item->next     = nullptr;
        head = item;

    }
    ~linkedList()
    {
        if (nullptr != head) {
            struct node* delItem = nullptr; 
            while (nullptr != head) {
                printf("~linkedList: head threadID = %p, deleted item = %p\n", 
                    head, delItem);
                delItem = head;
                head = head->next;
                free(delItem);
            }
        }
    }
    int8_t addItem(pthread_t threadID, int32_t commSocketPortNo)    // Returns 0 in case of success, -1 otherwise.
    {
       struct node *initialHead = nullptr;

        if (nullptr != head) {
            initialHead = head;
            while (nullptr != head->next) {
                head = head->next;
                // printf("addItem: head threadID = %lu, head next = %p\n", 
                //     head->threadID, head->next);
            }
            head->next = static_cast<struct node*>(malloc(sizeof(struct node)));
            head->next->commSocketPortNo = commSocketPortNo;
            head->next->threadID = threadID;
            head->next->next     = nullptr;
            head = initialHead;
        } else {
            struct node* item = 
                static_cast<struct node*>(malloc(sizeof(struct node)));
            if (nullptr == item) {
                throw;
            }
            item->commSocketPortNo = commSocketPortNo;
            item->threadID = threadID;
            item->next     = nullptr;
            head = item;
        } 
    }
    int8_t removeItem(int32_t commSocketPortNo) // Returns 0 in case of success, -1 otherwise.
    {
        int8_t status = -1;
        struct node *initialhead = head;
        struct node *prevItem = head;

        if (commSocketPortNo == initialhead->commSocketPortNo) {
            head = initialhead->next;
            free(initialhead);
            status = 0;
        } else {
            while (nullptr != head) {
                if (commSocketPortNo == head->commSocketPortNo) {                
                    prevItem->next = head->next;
                    free(head);
                    head = initialhead;
                    status = 0;
                    break;
                } else {
                    prevItem = head;
                    head = head->next;
                }
            }
            return status;
        }
    }
    int32_t countItems(void)         // Returns the item count in case of success, -1 otherwise.
    {
        int32_t itemCount = 0;
        struct node *item = head;

        while (nullptr != item) {
            ++itemCount;
            printf("countItems: node threadID = %lu, node = %p, itemCount = %d\n", 
                item->threadID, item, itemCount);
            item = item->next;
        }
        return itemCount;
    }
    struct node* getItem(int32_t commSocketPortNo) // Returns address of the item if it exists, nullptr otherwise.
    {
        struct node *item = head;

        while (nullptr != item) {
            if (commSocketPortNo == item->commSocketPortNo) {
                break;
            } else {
                item = item->next;
            }
        }
        return item;
    }
    linkedList& operator=(const linkedList& node) = delete; // Disable linked list assignment
    linkedList::node& operator=(const linkedList::node& node) = delete; // Enable element assignment, basically node assignment
    linkedList(const linkedList& node) = delete; // Disable copying
    
private:
    struct node *head;
};

}   /* extern "C" */
#endif /* __cplusplus */
#endif /* QUEUE_H_ */