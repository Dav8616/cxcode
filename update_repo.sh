#!/bin/bash

# =========================================================
# GIT REPO UPDATE SCRIPT
# This script performs the standard, safe 4-step Git workflow:
# 1. Add all changes
# 2. Commit (requires user input for the message)
# 3. Pull (fetches and merges remote changes)
# 4. Push (sends local commits to the remote)
# =========================================================

BRANCH="main"
REMOTE="origin"

echo "=========================================================="
echo " Starting Git Synchronization for branch: $BRANCH"
echo "=========================================================="

# 1. Stage all changes (modified and new files)
echo -e "\n---> STEP 1: Staging all modified and untracked files..."
git add .

# Check if there are any changes to commit
if git diff-index --quiet HEAD --; then
    echo "       (No changes detected. Skipping commit.)"
else
    # 2. Commit the staged changes
    echo -e "\n---> STEP 2: Committing staged changes..."
    
    # Prompt user for a commit message
    read -p "Enter Commit Message (REQUIRED): " COMMIT_MESSAGE

    if [ -z "$COMMIT_MESSAGE" ]; then
        echo "Error: Commit message cannot be empty. Aborting commit."
        exit 1
    fi

    git commit -m "$COMMIT_MESSAGE"

    if [ $? -ne 0 ]; then
        echo "Error during commit. Please review and fix."
        exit 1
    fi
fi


# 3. Pull changes from the remote to synchronize local history
echo -e "\n---> STEP 3: Pulling latest changes from $REMOTE/$BRANCH..."
# We use --no-rebase to ensure a standard merge commit is created if remote changes exist.
git pull $REMOTE $BRANCH --no-rebase

if [ $? -ne 0 ]; then
    echo "Warning: Pull resulted in conflicts or an error."
    echo "         Please resolve conflicts manually before running this script again."
    exit 1
fi


# 4. Push combined history to the remote
echo -e "\n---> STEP 4: Pushing local commits to $REMOTE/$BRANCH..."
git push $REMOTE $BRANCH

if [ $? -eq 0 ]; then
    echo -e "\n=========================================================="
    echo "✅ SUCCESS! Repository is fully synchronized."
    echo "=========================================================="
else
    echo -e "\n=========================================================="
    echo "❌ PUSH FAILED. Check network connection or remote status."
    echo "=========================================================="
    exit 1
fi
